/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "common/types.h"
#include "sys.h"
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "fsl_sys.h"
#include "fsl_dprc.h"
#include "fsl_dpci.h"
#include "fsl_dpci_event.h"
#include "fsl_dpci_drv.h"
#include "fsl_dpci_mng.h"
#include "fsl_sl_dprc_drv.h"
#include "fsl_sl_dbg.h"
#include "cmdif_client.h"
#include "cmdif_srv.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_icontext.h"
#include "fsl_spinlock.h"
#include "fsl_malloc.h"
#include "aiop_common.h"
#include "fsl_ep.h"
#include "fsl_ep_mng.h"
#include "fsl_sl_evmng.h"

/*************************************************************************/
#define DPCI_LOW_PR		1
#define DPCI_ID_FLG_SCANNED	1

#define CMDIF_Q_OPTIONS (DPCI_QUEUE_OPT_USER_CTX | DPCI_QUEUE_OPT_DEST)

#define CMDIF_FQD_CTX_GET \
	(((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fqd_ctx)

#define CMDIF_RX_CTX_GET \
	(LLLDW_SWAP((uint32_t)&CMDIF_FQD_CTX_GET, 0))

#define AMQ_BDI_SET(_offset, _width, _type, _arg) \
	(amq_bdi |= u32_enc((_offset), (_width), (_arg)))

#define AMQ_BDI_GET(_offset, _width, _type, _arg) \
	(*(_arg) = (_type)u32_dec(amq_bdi, (_offset), (_width)))

#define USER_CTX_SET(_offset, _width, _type, _arg) \
	(queue_cfg.user_ctx |= u64_enc((_offset), (_width), (_arg)))

#define USER_CTX_GET(_offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(rx_ctx, (_offset), (_width)))

#define CMDIF_DPCI_FQID(_OP, DPCI, FQID) \
do { \
	_OP(32,		32,	uint32_t,	DPCI); \
	_OP(0,		32,	uint32_t,	FQID); \
} while (0)


#define CMDIF_ICID_AMQ_BDI(_OP, ICID, AMQ_BDI) \
do { \
	_OP(16,		16,	uint16_t,	ICID); \
	_OP(0,		16,	uint16_t,	AMQ_BDI); \
} while (0)

#define MEM_SET(_ADDR, _SIZE, _VAL) \
	do { \
		for (i = 0; i < (_SIZE); i++) \
			((uint8_t *)_ADDR)[i] = _VAL; \
	} while (0)

#define DPCI_ENTRY_LOCK_W_TAKE(IND) \
	do { \
		cdma_mutex_lock_take((uint64_t)(&g_dpci_tbl.dpci_id[IND]), CDMA_MUTEX_WRITE_LOCK); \
	} while(0)

#define DPCI_ENTRY_LOCK_R_TAKE(IND) \
	do { \
		cdma_mutex_lock_take((uint64_t)(&g_dpci_tbl.dpci_id[IND]), CDMA_MUTEX_READ_LOCK); \
	} while(0)

#define DPCI_ENTRY_LOCK_RELEASE(IND) \
	do { \
		cdma_mutex_lock_release((uint64_t)(&g_dpci_tbl.dpci_id[IND])); \
	} while(0)


int dpci_drv_init();
void dpci_drv_free();


extern struct aiop_init_info g_init_data;

struct dpci_mng_tbl g_dpci_tbl = {0};

__COLD_CODE static void dpci_tbl_dump()
{
	int i;

	fsl_os_print("----------DPCI table: count = %d----------\n", 
	             g_dpci_tbl.count);
	for (i = 0; i < g_dpci_tbl.count; i++) {
		fsl_os_print("ID = 0x%x\t PEER ID = 0x%x\t IC = 0x%x\t flags = 0x%x\t\n",
		             g_dpci_tbl.dpci_id[i], g_dpci_tbl.dpci_id_peer[i], 
		             g_dpci_tbl.ic[i], g_dpci_tbl.flags[i]);
	}
}

__HOT_CODE int dpci_mng_find(uint32_t dpci_id)
{
	int i = 0;
	int count = 0;

	ASSERT_COND(dpci_id != DPCI_FQID_NOT_VALID);
	ASSERT_COND(g_dpci_tbl.count <= g_dpci_tbl.max);

	/* If in the middle of ADD or REMOVE then g_dpci_tbl.count be 1 higher
	 * than the number of valid entries, but it can't be lower */
	while ((count < g_dpci_tbl.count) && (i < g_dpci_tbl.max)) {

		if (g_dpci_tbl.dpci_id[i] != DPCI_FQID_NOT_VALID) {
			if (g_dpci_tbl.dpci_id[i] == dpci_id) {
				return i;
			}
			count++;
		}
		i++;
	}

	return -ENOENT;
}

__HOT_CODE int dpci_mng_peer_find(uint32_t dpci_id)
{
	int i = 0;
	int count = 0;

	ASSERT_COND(dpci_id != DPCI_FQID_NOT_VALID);
	ASSERT_COND(g_dpci_tbl.count <= g_dpci_tbl.max);

	/* If in the middle of ADD or REMOVE then g_dpci_tbl.count be 1 higher
	 * than the number of valid entries, but it can't be lower */
	while ((count < g_dpci_tbl.count) && (i < g_dpci_tbl.max)) {

		if (g_dpci_tbl.dpci_id[i] != DPCI_FQID_NOT_VALID) {
			if (g_dpci_tbl.dpci_id_peer[i] == dpci_id) {
				return i;
			}
			count++;
		}
		i++;
	}

	return -ENOENT;
}

static int dpci_entry_get()
{
	int i;

	for (i = 0; i < g_dpci_tbl.max; i++)
		if (g_dpci_tbl.dpci_id[i] == DPCI_FQID_NOT_VALID) {
			ASSERT_COND(g_dpci_tbl.ic[i] == DPCI_FQID_NOT_VALID);
			/* Must be last and atomic because the search 
			 * is not protected */
			atomic_incr32(&g_dpci_tbl.count, 1);
			return i;
		}

	return -ENOENT;
}

__COLD_CODE static void dpci_entry_delete(int ind)
{

	int i;

	g_dpci_tbl.dpci_id[ind] = DPCI_FQID_NOT_VALID;
	g_dpci_tbl.ic[ind] = DPCI_FQID_NOT_VALID;
	g_dpci_tbl.dpci_id_peer[ind] = DPCI_FQID_NOT_VALID;
	g_dpci_tbl.flags[ind] = 0;
	for (i = 0 ; i < DPCI_PRIO_NUM; i++)
		g_dpci_tbl.tx_queue[ind][i] = DPCI_FQID_NOT_VALID;
	/* Must be last and atomic because the search is not protected */
	atomic_decr32(&g_dpci_tbl.count, 1);
}

static inline void amq_bits_update(uint32_t id)
{
	uint32_t amq_bdi = 0;
	uint16_t amq_bdi_temp = 0;
	uint16_t pl_icid = PL_ICID_GET;

	if (g_dpci_tbl.dpci_id[id] == DPCI_FQID_NOT_VALID)
		return;

	ADD_AMQ_FLAGS(amq_bdi_temp, pl_icid);
	if (BDI_GET != 0)
		amq_bdi_temp |= CMDIF_BDI_BIT;

	CMDIF_ICID_AMQ_BDI(AMQ_BDI_SET, ICID_GET(pl_icid), amq_bdi_temp);

	/* Must be written last */
	g_dpci_tbl.ic[id] = amq_bdi;
	/* Don't update AIOP to AIOP DPCI 2 entries because it
	 * shouldn't change anyway */
}

/*
 * Set the peer id and the tx queues
 * Reset it if it is not yet available
 */
__COLD_CODE static int tx_peer_set(uint32_t ind, uint16_t token)
{

	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	struct dpci_tx_queue_attr tx_attr;
	struct dpci_peer_attr peer_attr;
	int i;
	uint32_t *tx = &g_dpci_tbl.tx_queue[ind][0];

	ASSERT_COND(tx);
	ASSERT_COND(dprc);

	g_dpci_tbl.dpci_id_peer[ind] = DPCI_FQID_NOT_VALID;
	for (i = 0; i < DPCI_PRIO_NUM; i++) {
		tx[i] = DPCI_FQID_NOT_VALID;
	}

	/* memset */
	MEM_SET(&tx_attr, sizeof(tx_attr), 0);
	MEM_SET(&peer_attr, sizeof(peer_attr), 0);

	err = dpci_get_peer_attributes(&dprc->io, token, &peer_attr);
	if (err || (peer_attr.peer_id == -1)) {
		sl_pr_err("Failed to get peer_id dpci id = %d err = %d\n",
		       g_dpci_tbl.dpci_id[ind], err);
		return -ENODEV;
	}

	g_dpci_tbl.dpci_id_peer[ind] = (uint32_t)peer_attr.peer_id;

	for (i = 0; i < peer_attr.num_of_priorities; i++) {
		err = dpci_get_tx_queue(&dprc->io, token, (uint8_t)i, &tx_attr);
		ASSERT_COND(!err);
		ASSERT_COND(tx_attr.fqid != DPCI_FQID_NOT_VALID);
		tx[i] = tx_attr.fqid;
	}

	return err;
}

__COLD_CODE static uint8_t num_priorities_get(struct fsl_mc_io *mc_io,
                                             uint16_t token)
{
	struct dpci_attr attr;
	uint8_t i;

	MEM_SET(&attr, sizeof(attr), 0);
	dpci_get_attributes(mc_io, token, &attr);
	return attr.num_of_priorities;
}

/* Stack size issue */
//#pragma optimization_level 2
#pragma inline_depth(0)
__COLD_CODE static void tx_user_context_set(struct mc_dprc *dprc, int ind,
                                            uint16_t token, uint8_t num_pr)
{
	uint8_t i;
	struct dpci_rx_queue_cfg queue_cfg;

	ASSERT_COND(num_pr >= 1);

	MEM_SET(&queue_cfg, sizeof(queue_cfg), 0);

	/*
	 * Set the tx queue in user context
	 */
	queue_cfg.dest_cfg.dest_type = DPCI_DEST_NONE;
	queue_cfg.options = CMDIF_Q_OPTIONS;
	for (i = 0; i < num_pr; i++) {
		queue_cfg.dest_cfg.priority = DPCI_LOW_PR - i;
		queue_cfg.user_ctx = 0;
		CMDIF_DPCI_FQID(USER_CTX_SET, ind, g_dpci_tbl.tx_queue[ind][i]);
		dpci_set_rx_queue(&dprc->io, token, i, &queue_cfg);
	}
}
#pragma inline_depth(smart)
//#pragma optimization_level reset

/* To be called upon connected event, assign even */
__COLD_CODE static int dpci_entry_init(uint32_t dpci_id, uint16_t token)
{
	int ind = -1;
	uint32_t amq_bdi = 0;
	int err = 0;

	CMDIF_ICID_AMQ_BDI(AMQ_BDI_SET, ICONTEXT_INVALID, ICONTEXT_INVALID);

	ind = dpci_entry_get();
	if (ind < 0) {
		pr_err("Not enough entries\n");
		return -ENOMEM;
	}

	ASSERT_COND((ind >= 0) && (ind < g_dpci_tbl.max));

	g_dpci_tbl.dpci_id[ind] = dpci_id;
	g_dpci_tbl.ic[ind] = amq_bdi;

	/* Updated DPCI peer if possible
	 * error is possible */
	err = tx_peer_set((uint32_t)ind, token);
#if 0
	/*
	 * Should be resolved by connected event or upon link change event
	 */
	if (!err) {
		/* Check AIOP DPCI to AIOP DPCI case 2 entries must be updated
		 * 1->2 and 2->1 */
		err = dpci_mng_find(g_dpci_tbl.dpci_id_peer[ind]);
		if (err >= 0) {
			/* If here then 2 AIOP DPCIs are connected */
			tx_peer_set((uint32_t)err, token);
			pr_debug("AIOP DPCI %d to AIOP DPCI %d\n",
			         dpci_id, g_dpci_tbl.dpci_id_peer[err]);
		}
	}
#endif

	return ind;
}

__COLD_CODE static int mc_intr_set(uint32_t dpci_id, struct mc_dprc *dprc,
                                   uint16_t token)
{
	struct dpci_irq_cfg irq_cfg;
	int err;
	uint32_t mask = DPCI_IRQ_EVENT_LINK_CHANGED | 
		DPCI_IRQ_EVENT_CONNECTED | 
		DPCI_IRQ_EVENT_DISCONNECTED;
	irq_cfg.addr = DPCI_EVENT;
	irq_cfg.val = dpci_id;
	irq_cfg.user_irq_id = 0;

	err = dpci_set_irq(&dprc->io, token, DPCI_IRQ_INDEX, &irq_cfg);
	if(err){
		pr_err("Failed to set irq for DP-CI%d\n", dpci_id);
		return err;
	}

	err = dpci_set_irq_mask(&dprc->io, 
	                        token,
	                        DPCI_IRQ_INDEX,
	                        mask);
	if(err){
		pr_err("Failed to set irq mask for DP-CI%d\n", dpci_id);
		return err;
	}

	err = dpci_clear_irq_status(&dprc->io, token, DPCI_IRQ_INDEX, mask);
	ASSERT_COND(!err);

	err = dpci_set_irq_enable(&dprc->io, token, DPCI_IRQ_INDEX, 1);
	if(err){
		pr_err("Failed to set irq enable for DP-CI%d\n", dpci_id);
		return err;
	}
	return 0;
}

__COLD_CODE int dpci_event_update_obj(uint32_t dpci_id)
{
	int err = 0;
	int ind = -1;
	int raise_event = 0;
	uint16_t token = 0xffff;
	struct mc_dprc *dprc;

	/*
	 * MC<->AIOP DPCI does not change and it is added and enabled by SL
	 */
	if (g_dpci_tbl.mc_dpci_id == dpci_id)
		return 0;

	DPCI_DT_LOCK_W_TAKE;

	ind = dpci_mng_find(dpci_id);
	if (ind < 0) {

		dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
		ASSERT_COND(dprc);
		err = dpci_open(&dprc->io, (int)dpci_id, &token);
		if (err) {
			DPCI_DT_LOCK_RELEASE;
			return err;
		}

		ind = dpci_entry_init(dpci_id, token);
		if (ind >= 0) {
			raise_event = 1;
		} else {
			pr_err("Add new DPCI 0x%x failed\n", dpci_id);
		}

		err = mc_intr_set(dpci_id, dprc, token);
		if (err) {
			pr_err("Failed to set irq for events\n");
		}

		err = dpci_close(&dprc->io, token);
		ASSERT_COND(!err);
	}

	if(ind >= 0) {
		ASSERT_COND(!(g_dpci_tbl.flags[ind] & DPCI_ID_FLG_SCANNED));
		/* flags are updated only during add/remove 
		 * event which are handled one at a time */
		g_dpci_tbl.flags[ind] |= DPCI_ID_FLG_SCANNED;
	}

	DPCI_DT_LOCK_RELEASE;

	dpci_tbl_dump();

	if (raise_event) {
		err = evmng_sl_raise_event(EVMNG_GENERATOR_AIOPSL,
		                           DPCI_EVENT_ADDED,
		                           (void *)dpci_id);
		if(err){
			pr_err("Failed to raise event for "
				"CI-%d.\n", dpci_id);
			return err;
		}
	}

	return err;
}

__COLD_CODE void dpci_event_handle_removed_objects()
{
	int i = 0;
	int count = 0;
	int err;

	ASSERT_COND(g_dpci_tbl.count <= g_dpci_tbl.max);

	DPCI_DT_LOCK_W_TAKE;

	while ((count < g_dpci_tbl.count) && (i < g_dpci_tbl.max)) {

		pr_debug("i=%d count=%d\n", i, count);

		if (g_dpci_tbl.dpci_id[i] != DPCI_FQID_NOT_VALID) {
			
			if (!(g_dpci_tbl.flags[i] & DPCI_ID_FLG_SCANNED)) {

				dpci_entry_delete(i);
				
				pr_debug("evmng_sl_raise_event\n");
				err = evmng_sl_raise_event(
					EVMNG_GENERATOR_AIOPSL,
					DPCI_EVENT_REMOVED,
					(void *)g_dpci_tbl.dpci_id[i]);
				if (err) {
					pr_err("Failed event DPCI-%d.\n",
					       g_dpci_tbl.dpci_id[i]);
				}
			}

			if (g_dpci_tbl.mc_dpci_id != g_dpci_tbl.dpci_id[i]) {
				/* flags are updated only during add/remove 
				 * event which are handled one at a time */
				g_dpci_tbl.flags[i] &= ~DPCI_ID_FLG_SCANNED;
				pr_debug("Cleared DPCI_ID_FLG_SCANNED\n");
			}

			count++;
		}

		i++;
	}

	DPCI_DT_LOCK_RELEASE;

	dpci_tbl_dump();
}


/*
 * The DPCI user context and AMQ bits are updated
 * This function is to be called only inside the open command and before
 * the AMQ bits had been changed to AIOP AMQ bits
 */
__COLD_CODE void dpci_mng_update(uint32_t ind)
{

	amq_bits_update(ind);

	dpci_tbl_dump();
}

__COLD_CODE int dpci_event_link_change(uint32_t dpci_id)
{
	int err = 0;
	int ind;
	int linkup;
	uint32_t status = 0;
	uint16_t token = 0xffff;
	uint8_t event_id;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	err = dpci_open(&dprc->io, (int)dpci_id, &token);
	if (err)
		return err;

	err = dpci_get_irq_status(&dprc->io,
	                          token,
	                          DPCI_IRQ_INDEX,
	                          &status);
	ASSERT_COND(!err);

	if (status & DPCI_IRQ_EVENT_CONNECTED) {
		event_id = DPCI_EVENT_CONNECTED;
		status = DPCI_IRQ_EVENT_CONNECTED;
	} else if (status & DPCI_IRQ_EVENT_DISCONNECTED) {
		event_id = DPCI_EVENT_DISCONNECTED;
		status = DPCI_IRQ_EVENT_DISCONNECTED;
	} else 	if (status & DPCI_IRQ_EVENT_LINK_CHANGED) {

		linkup = 0;
		err = dpci_get_link_state(&dprc->io, token, &linkup);
		ASSERT_COND(!err);

		if (linkup == 0)
			event_id = DPCI_EVENT_LINK_DOWN;
		else
			event_id = DPCI_EVENT_LINK_UP;

		status = DPCI_IRQ_EVENT_LINK_CHANGED;
	} else {
		return -ENOTSUP;
	}

	DPCI_DT_LOCK_W_TAKE;
	ind = dpci_mng_find(dpci_id);
	ASSERT_COND(ind >= 0);
	tx_peer_set((uint32_t)ind, token);
	tx_user_context_set(dprc, ind, token,
	                    num_priorities_get(&dprc->io,
	                                       token));
	DPCI_DT_LOCK_RELEASE;

	err = evmng_sl_raise_event(EVMNG_GENERATOR_AIOPSL, event_id,
	                           (void *)dpci_id);
	ASSERT_COND(!err);

	err = dpci_clear_irq_status(&dprc->io, token, DPCI_IRQ_INDEX, status);
	ASSERT_COND(!err);

	err = dpci_close(&dprc->io, token);
	ASSERT_COND(!err);

	pr_debug("Done DPCI EVENT\n");
	return err;
}

__HOT_CODE void dpci_mng_user_ctx_get(uint32_t *id, uint32_t *fqid)
{
	uint64_t rx_ctx = CMDIF_RX_CTX_GET;
	uint32_t _id;
	uint32_t _fqid;

	CMDIF_DPCI_FQID(USER_CTX_GET, (&_id), (&_fqid));

	if (id)
		*id = _id;
	if (fqid)
		*fqid = _fqid;
}

__HOT_CODE void dpci_mng_icid_get(uint32_t ind, uint16_t *icid, uint16_t *amq_bdi_out)
{
	uint32_t amq_bdi = g_dpci_tbl.ic[ind];

	/* 1. Atomic read of 4 bytes so that icid and amq are always in sync
	 * 2. Retrieve the icid and amq & bdi */

	CMDIF_ICID_AMQ_BDI(AMQ_BDI_GET, icid, amq_bdi_out);
}

__HOT_CODE void dpci_mng_tx_get(uint32_t ind, int pr, uint32_t *fqid)
{
	*fqid = g_dpci_tbl.tx_queue[ind][pr];
}

/* Stack size issue */
#pragma inline_depth(0)
__COLD_CODE int dpci_drv_enable(uint32_t dpci_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	int ind;
	uint16_t token = 0xffff;

	ASSERT_COND(dprc);

	/* Open must be the first action to lock the object */
	err = dpci_open(&dprc->io, (int)dpci_id, &token);
	if (err) {
		return err;
	}

	/*
	 * Update DPCI table tx and peer 
	 */
	DPCI_DT_LOCK_W_TAKE;

	ind = dpci_mng_find(dpci_id);
	if (ind >= 0) {
		tx_peer_set((uint32_t)ind, token);
		tx_user_context_set(dprc, ind, token,
		                    num_priorities_get(&dprc->io, token));
	} else {
		sl_pr_err("No DPCI in the table \n");		
	}

	DPCI_DT_LOCK_RELEASE;

	err = dpci_enable(&dprc->io, token);

	dpci_close(&dprc->io, token);

	return err; /* Error of enable matters */
}
#pragma inline_depth(smart)

__COLD_CODE int dpci_drv_disable(uint32_t dpci_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err = 0;
	uint16_t token = 0xffff;

	ASSERT_COND(dprc);

	err = dpci_open(&dprc->io, (int)dpci_id, &token);
	if (err) {
		return err;
	}

	err = dpci_disable(&dprc->io, token);
	if (err) {
		sl_pr_err("DPCI disable failed\n");
		dpci_close(&dprc->io, token);
		return err;
	}

	/*
	 * I don't update the table in order to allow the existing tasks
	 * to finish gracefully.
	 * The table will be updated upon
	 * dpci_drv_enable()/dpci_event_link_change()
	 */
	dpci_close(&dprc->io, token);

	return err;
}

__COLD_CODE static int dpci_tbl_create(int dpci_count)
{
	uint32_t size = 0;

	size = sizeof(struct dpci_mng_tbl);
	memset(&g_dpci_tbl, 0, size);

	size = sizeof(g_dpci_tbl.dpci_id);
	memset(&g_dpci_tbl.dpci_id, 0xff, size);

	size = sizeof(g_dpci_tbl.dpci_id_peer);
	memset(&g_dpci_tbl.dpci_id_peer, 0xff, size);

	size = sizeof(g_dpci_tbl.ic) ;
	memset(&g_dpci_tbl.ic, 0xff, size);

	size = sizeof(g_dpci_tbl.tx_queue);
	memset(&g_dpci_tbl.tx_queue, 0xff, size);

	g_dpci_tbl.count = 0;
	g_dpci_tbl.max = dpci_count;
	g_dpci_tbl.mc_dpci_id = 0xff;

	//mem_disp((void *)&g_dpci_tbl, sizeof(g_dpci_tbl));

	return 0;
}

__COLD_CODE static int dpci_for_mc_add(struct mc_dprc *dprc)
{
	struct dpci_cfg dpci_cfg;
	uint16_t dpci = 0;
	struct dpci_rx_queue_cfg queue_cfg;
	struct dprc_endpoint endpoint1 ;
	struct dprc_endpoint endpoint2;
	struct dpci_attr attr;
	uint8_t p = 0;
	int     err = 0;
	int     link_up = 0;

	memset(&queue_cfg, 0, sizeof(struct dpci_rx_queue_cfg));
	memset(&attr, 0, sizeof(attr));

	dpci_cfg.num_of_priorities = 2;

	err = dpci_create(&dprc->io, &dpci_cfg, &dpci);
	ASSERT_COND(!err);

	/* Get attributes just for dpci id fqids are not there yet */
	err = dpci_get_attributes(&dprc->io, dpci, &attr);
	ASSERT_COND(!err);

	/* Connect to dpci that belongs to MC */
	g_dpci_tbl.mc_dpci_id = (uint32_t)attr.id;
	pr_debug("MC dpci ID[%d] \n", g_init_data.sl_info.mc_dpci_id);
	pr_debug("AIOP dpci ID[%d] \n", g_dpci_tbl.mc_dpci_id);

	memset(&endpoint1, 0, sizeof(struct dprc_endpoint));
	memset(&endpoint2, 0, sizeof(struct dprc_endpoint));
	endpoint1.id = (int)g_init_data.sl_info.mc_dpci_id;
	endpoint1.interface_id = 0;
	strcpy(endpoint1.type, "dpci");

	endpoint2.id = attr.id;
	endpoint2.interface_id = 0;
	strcpy(endpoint2.type, "dpci");

	err = dprc_connect(&dprc->io, dprc->token, &endpoint1, &endpoint2);
	if (err) {
		pr_err("dprc_connect failed\n");
	}

	err = dpci_entry_init((uint32_t)attr.id, dpci);
	ASSERT_COND(err >= 0);
	/* MC dpci can't be removed */
	g_dpci_tbl.flags[err] |= DPCI_ID_FLG_SCANNED;

	err = dpci_close(&dprc->io, dpci);
	ASSERT_COND(!err);

	err = dpci_drv_enable((uint32_t)attr.id);
	return err;
}

__COLD_CODE static int dpci_event_cb(uint8_t generator_id, uint8_t event_id,
                                     uint64_t app_ctx, void *event_data)
{
	int err;

	UNUSED(app_ctx);

	ASSERT_COND((event_id == DPCI_EVENT) &&
	            (generator_id == EVMNG_GENERATOR_AIOPSL));

	pr_debug("DPCI EVENT \n");
	err = dpci_event_link_change((uint32_t)event_data);
	return err;
}

__COLD_CODE int dpci_drv_init()
{
	struct dprc_obj_desc dev_desc;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpci_mng_tbl *dpci_tbl = NULL;
	int dev_count  = 0;
	int dpci_count = 0;
	int err        = 0;
	int i          = 0;

	if (dprc == NULL) {
		pr_err("No AIOP root container \n");
		return -ENODEV;
	}

	if ((err = dprc_get_obj_count(&dprc->io, dprc->token, &dev_count)) != 0) {
		pr_err("Failed to get device count for RC auth_d = %d\n",
		       dprc->token);
		return err;
	}

	/* First count how many DPCI objects we have */
	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpci") == 0) {
			dpci_count++;
		}
	}

	err = dpci_tbl_create(DPCI_DYNAMIC_MAX);
	if (err != 0) {
		pr_err("Failed dpci_tbl_create() \n");
		return err;
	}

	err = dpci_for_mc_add(dprc);
	if (err) {
		pr_err("Failed to create and link AIOP<->MC DPCI \n");
		return err;
	}

	dpci_tbl_dump();

	err = evmng_irq_register(EVMNG_GENERATOR_AIOPSL,
	                         DPCI_EVENT,
	                         0,
	                         0,
	                         dpci_event_cb);
	if(err){
		pr_err("EVM registration for DPCI events failed %d\n",err);
		return -ENAVAIL;
	}
	else{
		pr_info("Registered to: dpci_event_cb\n");
	}

	return err;
}

__COLD_CODE void dpci_drv_free()
{
	/*
	 * TODO do I need to free anything ?
	 */
}

int dpci_drv_get_initial_presentation(uint8_t flags,
	struct ep_init_presentation* const init_presentation)
{
	uint16_t epid = AIOP_EPID_CMDIF_SERVER;

	ASSERT_COND((flags == DPCI_DRV_EP_SERVER) ||
	            (flags == DPCI_DRV_EP_CLIENT));

	if (flags == DPCI_DRV_EP_CLIENT)
		epid = AIOP_EPID_CMDIF_CLIENT;

	return ep_mng_get_initial_presentation(epid, init_presentation);
}


int dpci_drv_set_initial_presentation(uint8_t flags,
	const struct ep_init_presentation* const init_presentation)
{
	uint16_t epid = AIOP_EPID_CMDIF_SERVER;

	ASSERT_COND((flags == DPCI_DRV_EP_SERVER) ||
	            (flags == DPCI_DRV_EP_CLIENT));

	if (flags == DPCI_DRV_EP_CLIENT)
		epid = AIOP_EPID_CMDIF_CLIENT;

	return ep_mng_set_initial_presentation(epid, init_presentation);
}
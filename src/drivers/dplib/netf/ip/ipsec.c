/**************************************************************************//**
@File		ipsec.c

@Description	This file contains the AIOP IPSec implementation.
		
*//***************************************************************************/

#include "common/types.h"
//#include "common/spinlock.h"
#include "kernel/fsl_spinlock.h"
//#include "common/dbg.h"

#include "dplib/fsl_cdma.h"
#include "dplib/fsl_parser.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_l2.h"
#include "dplib/fsl_tman.h"
#include "dplib/fsl_ste.h"
#include "dplib/fsl_osm.h"
#include "header_modification.h"
#include "dplib/fsl_ipsec.h"
#include "ipsec.h"
#include "cdma.h"
#include "osm.h"
#include "system.h"

#ifdef AIOP_VERIF
#include "slab_stub.h"
#else
#include "slab.h"
#endif /* AIOP_VERIF */

/* Use this define to override the RTA and use a fixed debug descriptor */
//#define AIOPSL_IPSEC_DEBUG

//#include "general.h"

/* Note: GCC Extension should be enabled to support "typeof"
 * Otherwise it fails compilation of the RTA "ALIGN" macro.
 * #define ALIGN(x, a) (((x) + ((typeof(x))(a) - 1)) & ~((typeof(x))(a) - 1))
 * Alternative usage is "__typeof__":
 * #define ALIGN(x, a) \
 *	(((x) + ((__typeof__(x))(a) - 1)) & ~((__typeof__(x))(a) - 1))
*/

/* TODO: temporary fix to pr_debug due to RTA issue */
/* Define dummy print functions to override RTA compilation errors */
#ifndef pr_debug
#define use_dummy_pr_debug
void dummy_pr_debug (...);
void dummy_pr_debug (...) {}
#define pr_debug dummy_pr_debug 
#endif

#ifndef pr_err
#define use_dummy_pr_err
void dummy_pr_err (...);
void dummy_pr_err (...) {}
#define pr_err dummy_pr_err 
#endif

#include "rta.h"
#include "desc/ipsec.h"

#ifdef use_dummy_pr_debug
#undef pr_debug
#endif

#ifdef use_dummy_pr_err
#undef pr_err
#endif

/* SEC Era version for RTA */
enum rta_sec_era rta_sec_era = RTA_SEC_ERA_8;

/* Global parameters */
__SHRAM struct ipsec_global_params global_params;
__SHRAM struct ipsec_global_instance_params global_instance_params;

#ifdef AIOP_VERIF
__SHRAM uint64_t ipsec_debug_buf_addr; /* Global in Shared RAM */
__SHRAM uint32_t ipsec_debug_buf_size; /* Global in Shared RAM */
__SHRAM uint32_t ipsec_debug_buf_offset; /* Global in Shared RAM */
#endif


/**************************************************************************//**
* 	ipsec_init
*//****************************************************************************/
int ipsec_init(uint32_t max_sa_no) {

	global_params.sa_count = max_sa_no;
	global_params.asa_bpid = 1; // TMP
	global_params.desc_bpid = 1; // TMP
	global_params.tmi = 1; // TMP
	global_params.spinlock = 0;

	return 0;
} /* End of ipsec_init */


/**************************************************************************//**
*	ipsec_create_instance
*//****************************************************************************/
int ipsec_create_instance(
		uint32_t committed_sa_num,
		uint32_t max_sa_num,
		uint32_t instance_flags,
		uint8_t tmi_id,
		ipsec_instance_handle_t *instance_handle)
{
	
	int return_val;
	uint32_t tmp1 = committed_sa_num;
	uint32_t tmp2 = max_sa_num;
	uint32_t tmp3 = instance_flags;
	uint8_t tmp4 = tmi_id; 
	
	return_val = ipsec_init(committed_sa_num);
	
	*instance_handle = NULL;
	
	return IPSEC_SUCCESS; 

}
	
/**************************************************************************//**
*	ipsec_create_instance
*//****************************************************************************/
int ipsec_create_instance_real (
			uint32_t committed_sa_num,
			uint32_t max_sa_num,
			uint32_t instance_flags,
			uint8_t tmi_id,
			ipsec_instance_handle_t *instance_handle);
	
int ipsec_create_instance_real (
		uint32_t committed_sa_num,
		uint32_t max_sa_num,
		uint32_t instance_flags,
		uint8_t tmi_id,
		ipsec_instance_handle_t *instance_handle)
{
	int32_t return_val;
	
	// committed_sa_num for desc BPID size 512, alignment 64 B 
	// committed_sa_num for keys BPID
	// committed_sa_num for IPv6 outer header (TBD)
	// max num of tasks for ASA 
	
	int num_filled_buffs;
	
	struct ipsec_instance_params instance; 

	instance.sa_count = 0;
	instance.committed_sa_num = committed_sa_num;
	instance.max_sa_num = max_sa_num;
	instance.instance_flags = instance_flags;
	instance.tmi_id = tmi_id;

	/* Descriptor and Instance Buffers */
	return_val = slab_find_and_fill_bpid(
			(committed_sa_num + 1), /* uint32_t num_buffs */
			IPSEC_SA_DESC_BUF_SIZE, /* uint16_t buff_size */
			IPSEC_SA_DESC_BUF_ALIGN, /* uint16_t alignment */
			IPSEC_MEM_PARTITION_ID, /* TODO: TMP. uint8_t  mem_partition_id */
            &num_filled_buffs, /* int *num_filled_buffs */
            &(instance.desc_bpid)); /* uint16_t *bpid */
	
	if (return_val) {
		// TODO: call future slab release function per BPID
		// for all previously requested buffers
		return -ENOMEM;
	}
	
	/* TODO: ASA buffers should be shared for all instances */
	/* ASA Buffers */
	
	/* Check if instances counter is zero */
	/* If yes allocate ASA buffers */
	lock_spinlock((uint8_t *)&global_instance_params.spinlock);
		
	if (global_instance_params.instance_count == 0) {
		global_instance_params.instance_count++;
		unlock_spinlock((uint8_t *)&global_instance_params.spinlock);
		
		return_val = slab_find_and_fill_bpid(
				IPSEC_MAX_NUM_OF_TASKS, /* uint32_t num_buffs */
				IPSEC_MAX_ASA_SIZE, /* uint16_t buff_size */
				IPSEC_MAX_ASA_BUF_ALIGN, /* uint16_t alignment */
				IPSEC_MEM_PARTITION_ID, /* TODO: TMP. uint8_t  mem_partition_id */
	            &num_filled_buffs, /* int *num_filled_buffs */
	            &(instance.asa_bpid)); /* uint16_t *bpid */
		
		if (return_val) {
			// TODO: call future slab release function per BPID
			// for all previously requested buffers
			return -ENOMEM;
		}

	} else {
		global_instance_params.instance_count++;
		unlock_spinlock((uint8_t *)&global_instance_params.spinlock);
	}
	
	/* Allocate a buffer for the instance */
	return_val = (int32_t)cdma_acquire_context_memory(
		instance.desc_bpid,
		instance_handle); /* context_memory */ 
	
	if (return_val) {
		// TODO: return with correct error code 
		return IPSEC_ERROR;
	}
		
	/* Write the Instance to external memory */
	cdma_write(
			*instance_handle, /* ext_address */
			&instance, /* ws_src */
			(uint16_t)(sizeof(instance))); /* size */

	return IPSEC_SUCCESS; 
}


/**************************************************************************//**
*	ipsec_create_instance
*//****************************************************************************/
int ipsec_delete_instance(ipsec_instance_handle_t instance_handle);

int ipsec_delete_instance(ipsec_instance_handle_t instance_handle)
{
	int32_t return_val;
	uint32_t sa_count;

	cdma_read_with_mutex(
			instance_handle, /* uint64_t ext_address */
			CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
			&sa_count, /* void *ws_dst */
			sizeof(sa_count) /* uint16_t size */	
	);

	/* Check if all SAs were deleted */
	if (sa_count == 0) {
		
		/* Release the instance buffer */ 
		return_val = cdma_refcount_decrement_and_release(instance_handle);
		/* TODO: check for CDMA errors. Mind reference count zero status */
		
		/* TODO: return "committed + 1" buffers back to the slab */
		
		/* Check if instances counter is zero */
		/* If yes return ASA buffers to the slab */
		lock_spinlock((uint8_t *)&global_instance_params.spinlock);
		
		/* Error if instance counter is already zero */
		if (global_instance_params.instance_count == 0) {
			/* EPERM = 1, Operation not permitted */
			return -EPERM; /* TODO: what is the correct error code? */
		}
				
		global_instance_params.instance_count--;
		
		/* This is the last instance */
		if (global_instance_params.instance_count == 0) {
			unlock_spinlock((uint8_t *)&global_instance_params.spinlock);
			
			/* TODO: return IPSEC_MAX_NUM_OF_TASKS buffers back to the slab */
		
		} else {
			unlock_spinlock((uint8_t *)&global_instance_params.spinlock);
		}
		
		return IPSEC_SUCCESS;		
	} else {
		/* TODO: handle a case of instance delete before SAs full delete */
		
		/* EPERM = 1, Operation not permitted */
		return -EPERM; /* TODO: what is the correct error code? */
	}
}

/**************************************************************************//**
*	ipsec_get_buffer (TMP)
*//****************************************************************************/
int ipsec_get_buffer(ipsec_instance_handle_t instance_handle,
		ipsec_handle_t *ipsec_handle
	);

int ipsec_get_buffer(ipsec_instance_handle_t instance_handle,
		ipsec_handle_t *ipsec_handle)
{
	int return_val;
	struct ipsec_instance_params instance; 
	int num_filled_buffs;

	cdma_read_with_mutex(
			instance_handle, /* uint64_t ext_address */
			CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
			&instance, /* void *ws_dst */
			sizeof(instance) /* uint16_t size */	
	);

	if (instance.sa_count < instance.committed_sa_num) {
		instance.sa_count++;
		/* Write and release lock */
		cdma_write_with_mutex(
				instance_handle, /* uint64_t ext_address */
				CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
				&instance.sa_count, /* void *ws_dst */
				sizeof(instance.sa_count) /* uint16_t size */	
		);
		
		return_val = (int)cdma_acquire_context_memory(
				instance.desc_bpid,
				ipsec_handle); /* context_memory */

		/* Check if CDMA allocation failed */
		if (return_val) goto get_buffer_alloc_err;
		
	} else if (instance.sa_count < instance.max_sa_num) {
		instance.sa_count++;
		/* Write and release lock */
		cdma_write_with_mutex(
				instance_handle, /* uint64_t ext_address */
				CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
				&instance.sa_count, /* void *ws_dst */
				sizeof(instance.sa_count) /* uint16_t size */	
		);
		/* Descriptor and Instance Buffers */
		return_val = slab_find_and_fill_bpid(
				1, /* uint32_t num_buffs */
				IPSEC_SA_DESC_BUF_SIZE, /* uint16_t buff_size */
				IPSEC_SA_DESC_BUF_ALIGN, /* uint16_t alignment */
				IPSEC_MEM_PARTITION_ID, /* TODO: TMP. uint8_t  mem_partition_id */
	            &num_filled_buffs, /* int *num_filled_buffs */
	            &(instance.desc_bpid)); /* uint16_t *bpid */

		/* Check if Slab has no buffers */
		if (return_val) goto get_buffer_alloc_err;

		return_val = (int)cdma_acquire_context_memory(
				instance.desc_bpid,
				ipsec_handle); /* context_memory */
		
		/* Check if CDMA allocation failed */
		if (return_val) goto get_buffer_alloc_err;

	} else {
		/* Release lock */
		cdma_mutex_lock_release(instance_handle);
		return -ENOMEM;
	}
	
	return IPSEC_SUCCESS; 

get_buffer_alloc_err:
	cdma_read_with_mutex(
			instance_handle, /* uint64_t ext_address */
			CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
			&instance.sa_count, /* void *ws_dst */
			sizeof(instance.sa_count) /* uint16_t size */	
	);
	
	instance.sa_count--;
	
	cdma_write_with_mutex(
			instance_handle, /* uint64_t ext_address */
			CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
			&instance.sa_count, /* void *ws_dst */
			sizeof(instance.sa_count) /* uint16_t size */	
	);
	
	return -ENOMEM;
} /* End of ipsec_get_buffer */

/**************************************************************************//**
*	ipsec_release_buffer (TMP)
*//****************************************************************************/
int ipsec_release_buffer(ipsec_instance_handle_t instance_handle,
		ipsec_handle_t ipsec_handle
	);

int ipsec_release_buffer(ipsec_instance_handle_t instance_handle,
		ipsec_handle_t ipsec_handle)
{
	int32_t return_val;
	struct ipsec_instance_params instance; 

	cdma_read_with_mutex(
			instance_handle, /* uint64_t ext_address */
			CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
			&instance, /* void *ws_dst */
			sizeof(instance) /* uint16_t size */	
	);

	if (instance.sa_count > 0) {
		/* Release the buffer */ 
		return_val = cdma_refcount_decrement_and_release(ipsec_handle); 
		/* TODO: check for CDMA errors. Mind reference count zero status */
				
		/* If buffer taken from 'max' quanta, need to return to slab */
		if (instance.sa_count > instance.committed_sa_num) {
		
			/* TODO: return one buffer back to the slab */
		}
		
		instance.sa_count--;
		
		/* Write (just the counter ) and release lock */
		cdma_write_with_mutex(
				instance_handle, /* uint64_t ext_address */
				CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
				&instance.sa_count, /* void *ws_dst */
				sizeof(instance.sa_count) /* uint16_t size */	
		);
		return IPSEC_SUCCESS;
	} else {
		/* Release lock */
		cdma_mutex_lock_release(instance_handle);
		/* EPERM = 1, Operation not permitted */
		return -EPERM; /* TODO: what is the correct error code? */
	}
} /* End of ipsec_release_buffer */	
		

/**************************************************************************//**
@Function		ipsec_generate_encap_sd 

@Description	Generate SEC Shared Descriptor for Encapsulation
*//***************************************************************************/
void ipsec_generate_encap_sd(
		uint64_t sd_addr, /* Flow Context Address in external memory */
		struct ipsec_descriptor_params *params,
		uint32_t *sd_size) /* Shared descriptor Length */
{
	
	uint8_t cipher_type = 0;
	
	struct encap_pdb {
		struct ipsec_encap_pdb innerpdb;
		//uint32_t *outer_hdr; 
		uint32_t outer_hdr[5]; 
	} pdb;	
	
	uint32_t ws_shared_desc[64]; /* Temporary Workspace Shared Descriptor */
	
	/* Build PDB fields for the RTA */
	
	/* Check which method is it according to the key */
	switch (params->cipherdata.algtype) {
		case IPSEC_CIPHER_AES_CBC:
		case IPSEC_CIPHER_DES_IV64:
		case IPSEC_CIPHER_DES:
		case IPSEC_CIPHER_3DES:
		case IPSEC_CIPHER_AES_XTS: // TODO: check if this is correct
		case IPSEC_CIPHER_NULL: /* No usage of IV for null encryption */
			cipher_type = CIPHER_TYPE_CBC;
			break;
		case IPSEC_CIPHER_AES_CTR:
			cipher_type = CIPHER_TYPE_CTR;
			break;
		case IPSEC_CIPHER_AES_CCM8:
		case IPSEC_CIPHER_AES_CCM12:
		case IPSEC_CIPHER_AES_CCM16:
			cipher_type = CIPHER_TYPE_CCM;
			break;
		case IPSEC_CIPHER_AES_GCM8:
		case IPSEC_CIPHER_AES_GCM12:
		case IPSEC_CIPHER_AES_GCM16:
		case IPSEC_CIPHER_AES_NULL_WITH_GMAC:
			cipher_type = CIPHER_TYPE_GCM;
			break;
		default:
			cipher_type = CIPHER_TYPE_CBC; // TODO: check if this is correct
	}
	
	switch (cipher_type) {
		case CIPHER_TYPE_CBC:
			/* uint32_t iv[4] */
			pdb.innerpdb.cbc.iv[0] = params->encparams.cbc.iv[0];
			pdb.innerpdb.cbc.iv[1] = params->encparams.cbc.iv[1];
			pdb.innerpdb.cbc.iv[2] = params->encparams.cbc.iv[2];
			pdb.innerpdb.cbc.iv[3] = params->encparams.cbc.iv[3];
			break;
		case CIPHER_TYPE_CTR:
			/*	uint32_t ctr_nonce; */
			/*	uint32_t ctr_initial; */
			/*	uint32_t iv[2]; */
			pdb.innerpdb.ctr.ctr_nonce = params->encparams.ctr.ctr_nonce;
			pdb.innerpdb.ctr.ctr_initial = 0;
			pdb.innerpdb.ctr.iv[0] = params->encparams.ctr.iv[0];
			pdb.innerpdb.ctr.iv[1] = params->encparams.ctr.iv[1];
			break;
		case CIPHER_TYPE_CCM:
			/*	uint32_t salt; lower 24 bits */
			/*	uint8_t b0_flags; */
			/*	uint8_t ctr_flags; */
			/*	uint16_t ctr_initial; */
			/*	uint32_t iv[2]; */
			pdb.innerpdb.ccm.salt = params->encparams.ccm.salt;
			pdb.innerpdb.ccm.b0_flags = 0;
			pdb.innerpdb.ccm.ctr_flags = 0;
			pdb.innerpdb.ccm.ctr_initial = 0;
			pdb.innerpdb.ccm.iv[0] = params->encparams.ccm.iv[0];
			pdb.innerpdb.ccm.iv[1] = params->encparams.ccm.iv[1];
			break;
		case CIPHER_TYPE_GCM:
			/*	uint32_t salt; lower 24 bits */
			/*	uint32_t rsvd1; */
			/*	uint32_t iv[2]; */
			pdb.innerpdb.gcm.salt = params->encparams.gcm.salt;
			pdb.innerpdb.gcm.rsvd1 = 0;
			pdb.innerpdb.gcm.iv[0] = params->encparams.gcm.iv[0];
			pdb.innerpdb.gcm.iv[1] = params->encparams.gcm.iv[1];
			break;
		default:
			pdb.innerpdb.cbc.iv[0] = 0;
			pdb.innerpdb.cbc.iv[1] = 0;
			pdb.innerpdb.cbc.iv[2] = 0;
			pdb.innerpdb.cbc.iv[3] = 0;
	}
	
	pdb.innerpdb.hmo = 
		(uint8_t)(((params->encparams.options) & IPSEC_ENC_PDB_HMO_MASK)>>8);
	pdb.innerpdb.options = 
		(uint8_t)((((params->encparams.options) & IPSEC_PDB_OPTIONS_MASK)) |
		IPSEC_ENC_PDB_OPTIONS_OIHI_PDB /* outer header from PDB */ 
		);

	// TODO: Program options[outFMT] for decapsulation
	
	//union {
	//	uint8_t ip_nh;	/* next header for legacy mode */
	//	uint8_t rsvd;	/* reserved for new mode */
	//};
	pdb.innerpdb.rsvd = 0;
				
	//union {
	//	uint8_t ip_nh_offset;	/* next header offset for legacy mode */
	//	uint8_t aoipho;		/* actual outer IP header offset for
	//				 * new mode */
	//};
	pdb.innerpdb.aoipho = 0;

	pdb.innerpdb.seq_num_ext_hi = params->encparams.seq_num_ext_hi;
	pdb.innerpdb.seq_num = params->encparams.seq_num;
	
	pdb.innerpdb.spi = params->encparams.spi;
		
	pdb.innerpdb.rsvd2 = 0;

	pdb.innerpdb.ip_hdr_len = params->encparams.ip_hdr_len;
	//pdb.outer_hdr = params->encparams.outer_hdr;

	// TODO: TMP workaround due to variable size array in the RTA PDB
	pdb.outer_hdr[0] = *(params->encparams.outer_hdr + 0);
	pdb.outer_hdr[1] = *(params->encparams.outer_hdr + 1);
	pdb.outer_hdr[2] = *(params->encparams.outer_hdr + 2);
	pdb.outer_hdr[3] = *(params->encparams.outer_hdr + 3);
	pdb.outer_hdr[4] = *(params->encparams.outer_hdr + 4);

	/* Call RTA function to build an encap descriptor */
	if (params->flags & IPSEC_FLG_TUNNEL_MODE) {
		/* Tunnel mode, SEC "new thread" */	
		cnstr_shdsc_ipsec_new_encap(
			(uint32_t *)(ws_shared_desc), /* uint32_t *descbuf */
			(unsigned *)sd_size, /* unsigned *bufsize */
			IPSEC_SEC_POINTER_SIZE, /* unsigned short ps */
			((struct ipsec_encap_pdb *)(&pdb)), /* PDB */
			(struct alginfo *)(&(params->cipherdata)),
			(struct alginfo *)(&(params->authdata)) 
		);
	} else {
		/* Transport mode, SEC legacy new thread */
		cnstr_shdsc_ipsec_encap(
			(uint32_t *)(ws_shared_desc), /* uint32_t *descbuf */
			(unsigned *)sd_size, /* unsigned *bufsize */
			IPSEC_SEC_POINTER_SIZE, /* unsigned short ps */
			((struct ipsec_encap_pdb *)(&pdb)), /* PDB */
			(struct alginfo *)(&(params->cipherdata)),
			(struct alginfo *)(&(params->authdata)) 
		);
	}	
	
	/* Write the descriptor to external memory */
	cdma_write(
			sd_addr, /* ext_address */
			ws_shared_desc, /* ws_src */
			(uint16_t)((*sd_size)<<2)); /* sd_size is in 32-bit words */
	
} /* End of ipsec_generate_encap_sd */

/**************************************************************************//**
@Function		ipsec_generate_decap_sd 

@Description	Generate SEC Shared Descriptor for Encapsulation
*//***************************************************************************/
void ipsec_generate_decap_sd(
		uint64_t sd_addr, /* Flow Context Address in external memory */
		struct ipsec_descriptor_params *params,
		uint32_t *sd_size) /* Shared descriptor Length */
{
	
	uint8_t cipher_type = 0;
	
	struct ipsec_decap_pdb pdb;

	uint32_t ws_shared_desc[64]; /* Temporary Workspace Shared Descriptor */

	/* Build PDB fields for the RTA */
	
	/* Check which method is it according to the key */
	switch (params->cipherdata.algtype) {
		case IPSEC_CIPHER_AES_CBC:
		case IPSEC_CIPHER_DES_IV64:
		case IPSEC_CIPHER_DES:
		case IPSEC_CIPHER_3DES:
		case IPSEC_CIPHER_AES_XTS: // TODO: check if this is correct
		case IPSEC_CIPHER_NULL: /* No usage of IV for null encryption */
			cipher_type = CIPHER_TYPE_CBC;
			break;
		case IPSEC_CIPHER_AES_CTR:
			cipher_type = CIPHER_TYPE_CTR;
			break;
		case IPSEC_CIPHER_AES_CCM8:
		case IPSEC_CIPHER_AES_CCM12:
		case IPSEC_CIPHER_AES_CCM16:
			cipher_type = CIPHER_TYPE_CCM;
			break;
		case IPSEC_CIPHER_AES_GCM8:
		case IPSEC_CIPHER_AES_GCM12:
		case IPSEC_CIPHER_AES_GCM16:
		case IPSEC_CIPHER_AES_NULL_WITH_GMAC:
			cipher_type = CIPHER_TYPE_GCM;
			break;
		default:
			cipher_type = CIPHER_TYPE_CBC; // TODO: check if this is correct
	}
		/*----------------------------------*/
		/* 	ipsec_generate_decap_sd			*/
		/*----------------------------------*/
	
	switch (cipher_type) {
		case CIPHER_TYPE_CBC:
			/* uint32_t rsvd[2]; */
			pdb.cbc.rsvd[0] = 0;
			pdb.cbc.rsvd[1] = 0;
            break;
		case CIPHER_TYPE_CTR:
			/* uint32_t salt; */
			/* uint32_t ctr_initial; */
			pdb.ctr.salt = params->decparams.ctr.salt;
			pdb.ctr.ctr_initial = params->decparams.ctr.ctr_initial;
			break;
		case CIPHER_TYPE_CCM:
			/* uint32_t salt; */
			/* uint8_t iv_flags; */
			/* uint8_t ctr_flags; */
			/* uint16_t ctr_initial; */
			pdb.ccm.salt = params->decparams.ccm.salt;
			pdb.ccm.iv_flags = 0;
			pdb.ccm.ctr_flags = 0;
			pdb.ccm.ctr_initial = 0;
			break;
		case CIPHER_TYPE_GCM:
			/* uint32_t salt; */
			/* uint32_t resvd; */
			pdb.gcm.salt = params->decparams.gcm.salt;
			pdb.gcm.resvd = 0;
			break;
		default:
			pdb.cbc.rsvd[0] = 0;
			pdb.cbc.rsvd[1] = 0;
	}
	
			/*----------------------------------*/
			/* 	ipsec_generate_decap_sd			*/
			/*----------------------------------*/

	
	/* uint16_t ip_hdr_len : 
	 * 		HMO (upper nibble)
	 * 		IP header length (lower 3 nibbles) is not relevant for tunnel
	 * 		and will be set by DPOVRD for transport */
	pdb.ip_hdr_len = 
			((params->decparams.options) & IPSEC_DEC_PDB_HMO_MASK);

	pdb.options = 
		(uint8_t)(((params->decparams.options) & IPSEC_PDB_OPTIONS_MASK));
	
	if (params->flags & IPSEC_FLG_TUNNEL_MODE) {
		pdb.options |= IPSEC_DEC_OPTS_ETU;
	}
	
	//union {
	//	uint8_t ip_nh_offset;	/* next header offset for legacy mode */
	//	uint8_t aoipho;		/* actual outer IP header offset for
	//				 * new mode */
	//};
	pdb.aoipho = 0; /* Will be set by DPOVRD */

	pdb.seq_num_ext_hi = params->decparams.seq_num_ext_hi;
	pdb.seq_num = params->decparams.seq_num;
	
	/* uint32_t anti_replay[4]; */
	pdb.anti_replay[0] = 0;
	pdb.anti_replay[1] = 0;
	pdb.anti_replay[2] = 0;
	pdb.anti_replay[3] = 0;

	
	/* Call RTA function to build an encap descriptor */
	if (params->flags & IPSEC_FLG_TUNNEL_MODE) {
		/* Tunnel mode, SEC "new thread" */	
		cnstr_shdsc_ipsec_new_decap(
			(uint32_t *)(ws_shared_desc), /* uint32_t *descbuf */
			(unsigned *)sd_size, /* unsigned *bufsize */
			IPSEC_SEC_POINTER_SIZE, /* unsigned short ps */
			&pdb, /* struct ipsec_encap_pdb *pdb */
			(struct alginfo *)(&(params->cipherdata)),
			(struct alginfo *)(&(params->authdata)) 
		);
	} else {
		/* Transport mode, SEC legacy new thread */
		cnstr_shdsc_ipsec_decap(
			(uint32_t *)(ws_shared_desc), /* uint32_t *descbuf */
			(unsigned *)sd_size, /* unsigned *bufsize */
			IPSEC_SEC_POINTER_SIZE, /* unsigned short ps */
			&pdb, /* struct ipsec_encap_pdb *pdb */
			(struct alginfo *)(&(params->cipherdata)),
			(struct alginfo *)(&(params->authdata)) 
		);
	}	
	
	/* Write the descriptor to external memory */
	cdma_write(
			sd_addr, /* ext_address */
			ws_shared_desc, /* ws_src */
			(uint16_t)((*sd_size)<<2)); /* sd_size is in 32-bit words */
	
} /* End of ipsec_generate_decap_sd */

/**************************************************************************//**
@Function		ipsec_generate_flc 

@Description	Generate SEC Flow Context Descriptor
*//***************************************************************************/
void ipsec_generate_flc(
		uint64_t flc_address, /* Flow Context Address in external memory */
		uint16_t spid, /* Storage Profile ID of the SEC output frame */
		uint32_t sd_size) /* Shared descriptor Length  in words*/
{
	
	struct ipsec_flow_context flow_context;

	/* TODO: temporary storage profiles implementation */
	extern struct storage_profile storage_profiles[NUM_OF_SP];
	uint64_t *sp_addr =  (uint64_t *)(&storage_profiles[spid]);
			
	/* Word 0 */
	flow_context.word0_sdid = 0; //TODO: how to get this value? 
	flow_context.word0_res = 0; 

	/* Word 1 */
	/* 5-0 SDL = Shared Descriptor length, 7-6 reserved */
	/* SDL is encoded in terms of 32-bit descriptor command words */ 
	flow_context.word1_sdl = (uint8_t)(sd_size & 0x000000FF);
	
	flow_context.word1_bits_15_8 = 0; /* 11-8 CRID, 14-12 reserved, 15 CRJD */
	flow_context.word1_bits23_16 = 0; /* 16	EWS,17 DAC,18-20?, 23-21 reserved */
	flow_context.word1_bits31_24 = 0; /* 24 RSC (not used for AIOP), 
		25 RBMT (not used for AIOP), 31-26 reserved */
	// TODO: check regarding EWS in buffer reuse mode
	
	/* word 2  RFLC[31-0] */
	flow_context.word2_rflc_31_0 = 0; /* Not used for AIOP */

	/* word 3  RFLC[63-32] */
	flow_context.word3_rflc_63_32 = 0; /* Not used for AIOP */

	/* word 4 */ /* Not used, should be NULL */
	flow_context.word4_iicid = 0; /* 15-0  IICID */
	flow_context.word4_oicid = 0; /* 31-16 OICID */
	
	/* word 5 */ 	
	flow_context.word5_7_0 = 0; /* 23-0 OFQID, not used for AIOP */
	flow_context.word5_15_8 = 0;
	flow_context.word5_23_16 = 0;
	/* 31-30 ICR = 2. AIOP is a trusted user - no need for any restrictions. */
	flow_context.word5_31_24 = 0x40;
						/* 24 OSC : not used for AIOP */
						/* 25 OBMT : not used for AIOP */
						/* 29-26 reserved */
						/* 31-30 ICR */
	/* word 6 */
	flow_context.word6_oflc_31_0 = 0; /* Not used for AIOP */
	
	/* word 7 */
	flow_context.word7_oflc_63_32 = 0; /* Not used for AIOP */
	
	/* Storage profile format:
	* 0x00 IP-Specific Storage Profile Information 
	* 0x08 Frame Format and Data Placement Controls 
	* 0x10 Buffer Pool 2, Buffer Pool 1 Attributes and Controls 
	* 0x18 Buffer Pool 4, Buffer Pool 3 Attributes and Controls
	* 
	* Only The data from offset 0x08 and 0x10 is copied to SEC flow context 
	*/
	/* Copy the standard Storage Profile to Flow Context words 8-15 */
	flow_context.storage_profile[0] = *(sp_addr + 1);
	flow_context.storage_profile[1] = *(sp_addr + 2);
	flow_context.storage_profile[2] = 0;
	flow_context.storage_profile[3] = 0;

	/* Write the Flow Context to external memory with CDMA */
	cdma_write(
			flc_address, /* ext_address */
			&flow_context, /* ws_src */
			IPSEC_FLOW_CONTEXT_SIZE); /* uint16_t size */
	
} /* End of ipsec_generate_flc */


/**************************************************************************//**
@Function		ipsec_generate_sa_params 

@Description	Generate and store the functional module internal parameter
*//***************************************************************************/
void ipsec_generate_sa_params(
		struct ipsec_descriptor_params *params, 
		ipsec_handle_t ipsec_handle) /* Parameters area (start of buffer) */
{
	
	struct ipsec_sa_params sap;
	
	/* Descriptor Part #1 */
	sap.sap1.flags = params->flags; // TMP 
		/* 	transport mode, UDP encap, pad check, counters enable, 
					outer IP version, etc. 4B */
	sap.sap1.status = 0; /* 	lifetime expiry, semaphores	*/

	/* UDP Encap for transport mode */
	sap.sap1.udp_src_port = 0; /* UDP source for transport mode. TMP */
	sap.sap1.udp_dst_port = 0; /* UDP destination for transport mode. TMP */

	/* Extended sequence number enable */
	sap.sap1.esn = (uint8_t)(((params->encparams.options) & 
					IPSEC_PDB_OPTIONS_MASK & IPSEC_ESN_MASK));

	sap.sap1.anti_replay_size = /* none/32/64/128 */ 
			(uint8_t)(((params->encparams.options) & 
					IPSEC_PDB_OPTIONS_MASK & IPSEC_ARS_MASK));
		
		/* new/reuse (for ASA copy). TMP */
	sap.sap1.sec_buffer_mode = IPSEC_SEC_NEW_BUFFER_MODE; 

	sap.sap1.output_spid = (uint8_t)(params->spid);

	sap.sap1.soft_byte_limit = params->soft_kilobytes_limit; 
	sap.sap1.soft_packet_limit = params->soft_packet_limit; 
	sap.sap1.hard_byte_limit = params->hard_kilobytes_limit; 
	sap.sap1.hard_packet_limit = params->hard_packet_limit; 
		
	sap.sap1.byte_counter = 0; /* Encrypted/decrypted bytes counter */
	sap.sap1.packet_counter = 0; /*	Packets counter */

	/* Set valid flag */
	sap.sap1.valid = 1; /* descriptor valid. */
	
	/* Descriptor Part #2 */
	sap.sap2.sec_callback_func = (uint32_t)params->lifetime_callback;
	sap.sap2.sec_callback_arg = params->callback_arg;
		
	// TODO: init one-shot timers according to:
	// soft_seconds_limit; 
	// hard_seconds_limit; 
	sap.sap2.soft_tmr_handle = NULL; /* Soft seconds timer handle, TMP */
	sap.sap2.hard_tmr_handle = NULL; /* Hard seconds timer handle, TMP */

	/* Get timestamp from TMAN */
	tman_get_timestamp(&(sap.sap1.timestamp));
	
	/* Store to external memory with CDMA */
	cdma_write(
			ipsec_handle, /* uint64_t ext_address */
			&sap, /* void *ws_src */
			(uint16_t)(sizeof(sap)) /* uint16_t size */
			);
	
} /* End of ipsec_generate_sa_params */

/**************************************************************************//**
*	ipsec_add_sa_descriptor
*//****************************************************************************/

/*                 SA Descriptor Structure
 * ------------------------------------------------------
 * |  ipsec_sa_params                 | 128 bytes       |
 * ------------------------------------------------------
 * | sec_flow_context                 | 64 bytes        |
 * -----------------------------------------------------
 * | sec_shared_descriptor            | Up to 256 bytes |
 * ------------------------------------------------------
 * | Replacement Job Descriptor (TBD) |                 |
 * ------------------------------------------------------
 * 
 * ipsec_sa_params - Parameters used by the IPsec functional module	128 bytes
 * sec_flow_context	- SEC Flow Context. 64 bytes
 * 			Should be 64-byte aligned for optimal performance.	
 * sec_shared_descriptor - Shared descriptor. Up to 256 bytes
 * Replacement Job Descriptor (RJD) for Peer Gateway Adaptation 
 * (Outer IP change)	TBD 
*/

int ipsec_add_sa_descriptor(
		struct ipsec_descriptor_params *params,
		ipsec_instance_handle_t instance_handle,
		ipsec_handle_t *ipsec_handle)
{

	int return_val;
	uint64_t sd_addr;
	uint32_t sd_size; /* shared descriptor size, set by the RTA */
	
	ipsec_instance_handle_t tmp1 = instance_handle;

	/* Create a shared descriptor */
	
	/* Check if SAs counter reached zero */
	/* If not, decrement SA counter. If yes return with error */
	lock_spinlock((uint8_t *)&global_params.spinlock);
		
	if (global_params.sa_count == 0) {
		unlock_spinlock((uint8_t *)&global_params.spinlock);
		return -1; // TODO add error code
	} else {
		global_params.sa_count--;
		unlock_spinlock((uint8_t *)&global_params.spinlock);
	}
	
	/* Allocate a buffer for the FM parameters, 
	 * Flow Context and Shared Descriptor with CDMA. */
	/* allocate a buffer with the CDMA */
	return_val = (int)cdma_acquire_context_memory(
		//0, /* TMP size; will be removed in future arch def */
		global_params.desc_bpid,
		ipsec_handle); /* context_memory */ 
	
	/* Check for allocation error */
	if (return_val) {
		// TODO: decrement SA counter
		return return_val;
	}
		
	// TODO: Optionally allocate a buffer for the key, if not inline.
	
	/* TODO: Clear the buffer area? */
	 
	/* Shared Descriptor address (adjacent to the flow context) */ 
	sd_addr = ((*ipsec_handle) + IPSEC_INTERNAL_PARMS_SIZE + 
			IPSEC_FLOW_CONTEXT_SIZE); 
	
	/* Build a shared descriptor with the RTA library */
	/* The RTA creates the descriptor and stores it in the memory 
		with CDMA commands. */
	if (params->direction == IPSEC_DIRECTION_INBOUND) {
		ipsec_generate_decap_sd(sd_addr,params, &sd_size);
	} else {
		ipsec_generate_encap_sd(sd_addr,params, &sd_size);
	}
	
	
	/* Generate the SEC Flow Context descriptor and write to memory with CDMA */
	ipsec_generate_flc(
			((*ipsec_handle) + IPSEC_INTERNAL_PARMS_SIZE), 
				/* Flow Context Address in external memory */
			params->spid, /* Storage Profile ID of the SEC output frame */
			sd_size); /* Shared descriptor size in words */
	
	/*	Prepare descriptor parameters:
	 * Kilobytes and packets lifetime limits.
	 * Modes indicators and other flags */
	/* Store the descriptor parameters to memory (CDMA write). */
	ipsec_generate_sa_params(
			params,
			*ipsec_handle); /* Parameters area (start of buffer) */
	
	/* Create one-shot TMAN timers for the soft and hard seconds lifetime 
	 * limits, with callback to internal function 
	 * (including the descriptor handle and soft/hard indication arguments). */
	
	/* Success, handle returened. */
	return IPSEC_SUCCESS;

add_sa_descriptor_error:
	// TODO: decrement SA counter
	/* Release the buffer */ 
	cdma_refcount_decrement_and_release(*ipsec_handle); /* context_memory */
	//TODO: what if CDMA release failed?
	return return_val; /* Return the previous error */
	
} /* End of ipsec_add_sa_descriptor */

/**************************************************************************//**
*	ipsec_del_sa_descriptor
*//****************************************************************************/
int ipsec_del_sa_descriptor(
		ipsec_handle_t ipsec_handle)
{

	int return_val;
	
	// TODO: Read descriptor with CDMA.
	// TODO Delete the timers; take care of callbacks in the middle of operation.
	
	/* Flush all the counter updates that are pending in the 
	 * statistics engine request queue. */
	ste_barrier();

	/* Release the buffer */ 
	return_val = (int)cdma_refcount_decrement_and_release(
		ipsec_handle); /* context_memory */ 
	
	// TODO: 
	// 1. Check that all frames are closed (reference count)
	// 2. Add timer delay for tasks that are in an interim state
	// (called by the application but did npt enter the SL yet)
	// If there were open frames do another ste_barrier();
	
	if (return_val != CDMA_REFCOUNT_DECREMENT_TO_ZERO) { /* error */
		//TODO: what if CDMA release failed?
		return IPSEC_ERROR; // TMP
	} else { /* success */
		atomic_incr32((int32_t *)(&(global_params.sa_count)), 1);
		return IPSEC_SUCCESS; // TMP
	}
	
} /* End of ipsec_del_sa_descriptor */

/**************************************************************************//**
* ipsec_frame_encrypt
*//****************************************************************************/
int ipsec_frame_encrypt(
		ipsec_handle_t ipsec_handle,
		uint32_t *enc_status
		)
{
	int return_val;
	uint8_t *last_etype_pointer;
	uint64_t eth_header[5]; /* Ethernet header place holder, 5*8=40 bytes */ 
	uint8_t eth_length = 0; /* Ethernet header length and indicator */ 
	uint64_t orig_flc;
	uint32_t orig_frc;
	uint64_t *eth_pointer_default;
	uint32_t byte_count;
	uint16_t checksum;
	uint8_t dont_encrypt = 0;

	struct ipsec_sa_params_part1 sap1; /* Parameters to read from ext buffer */
	struct scope_status_params scope_status;

	/* Increment the reference counter */
	cdma_refcount_increment(ipsec_handle);
	
	*enc_status = 0; /* Initialize */
	
	/* 	Outbound frame encryption and encapsulation (ipsec_frame_encrypt) 
	 * � Simplified Flow */

	/* 	2.	Read relevant descriptor fields with CDMA. */
	cdma_read(
			&sap1, /* void *ws_dst */
			ipsec_handle, /* uint64_t ext_address */
			sizeof(sap1) /* uint16_t size */
			);

	/*---------------------*/
	/* ipsec_frame_encrypt */
	/*---------------------*/
	
	/* 	3.	Check that hard kilobyte/packet/seconds lifetime limits have 
	 * not expired. If expired, return with error and go to END */
	/* The seconds lifetime status is checked in the params[status] 
	 * and the kilobyte/packet status is checked from the params[counters].
	 * This is done to avoid doing mutex lock for kilobyte/packet status */
	
	/* Seconds Lifetime */
	if (sap1.flags & IPSEC_FLG_LIFETIME_SEC_CNTR_EN) {
		if (sap1.status & IPSEC_STATUS_SOFT_SEC_EXPIRED) {
			*enc_status |= IPSEC_STATUS_SOFT_SEC_EXPIRED;
		}
		if (sap1.status & IPSEC_STATUS_HARD_SEC_EXPIRED) {
			*enc_status |= IPSEC_STATUS_HARD_SEC_EXPIRED;
			dont_encrypt = 1;
		}
	}
	
	/* KB lifetime counters */
	if (sap1.flags & IPSEC_FLG_LIFETIME_KB_CNTR_EN) {
		if (sap1.byte_counter >= sap1.soft_byte_limit) {
			*enc_status |= IPSEC_STATUS_SOFT_KB_EXPIRED;
		}
		if (sap1.byte_counter >= sap1.hard_byte_limit) {
			*enc_status |= IPSEC_STATUS_HARD_KB_EXPIRED;
			dont_encrypt = 1;
		}
	}
	
	/* Packets lifetime counters*/
	if (sap1.flags & IPSEC_FLG_LIFETIME_PKT_CNTR_EN) {

		if (sap1.packet_counter >= sap1.soft_packet_limit) {
			*enc_status |= IPSEC_STATUS_SOFT_PACKET_EXPIRED;
		}
		if (sap1.packet_counter >= sap1.hard_packet_limit) {
			*enc_status |= IPSEC_STATUS_HARD_PACKET_EXPIRED;
			dont_encrypt = 1;
		}
	}
	
	if (dont_encrypt) {
		return_val = IPSEC_ERROR; // TODO: TMP
		goto encrypt_end;
	}
	
		/*---------------------*/
		/* ipsec_frame_encrypt */
		/*---------------------*/
	
	/* 	4.	Identify if L2 header exist in the frame: */
	/* Check if Ethernet/802.3 MAC header exist and remove it */
	if (PARSER_IS_ETH_MAC_DEFAULT()) { /* Check if Ethernet header exist */
		
		/* For tunnel mode, update the Ethertype field according to the 
		 * outer header (IPv4/Ipv6), since after SEC encryption
		 * the parser results are not valid any more */
		
		/* Get the pointer to last EtherType */
		last_etype_pointer = PARSER_GET_LAST_ETYPE_POINTER_DEFAULT();
		
		/* Update the Ethertype according to the outher IP header */
		// TODO. Currently this is always IPv4
		
		/* Save Ethernet header. Note: no swap */
		/* up to 6 VLANs x 4 bytes + 14 regular bytes */
		eth_pointer_default = (uint64_t *)PARSER_GET_ETH_POINTER_DEFAULT();
		
		eth_header[0] = *(eth_pointer_default + 0);
		eth_header[1] = *(eth_pointer_default + 1);
		eth_header[2] = *(eth_pointer_default + 2);
		eth_header[3] = *(eth_pointer_default + 3);
		eth_header[4] = *(eth_pointer_default + 4);
		
		//TODO: debug info
#ifdef AIOP_VERIF
		if (ipsec_debug_buf_addr != NULL) {
			/* Write the debug info to external memory */
			cdma_write(
				(ipsec_debug_buf_addr + ipsec_debug_buf_offset), /* ext_address */
				&eth_header, /* ws_src */
				40); /* size */
			if (ipsec_debug_buf_offset <= (ipsec_debug_buf_size-64)) {
				ipsec_debug_buf_offset += 64;
			}
		}
#endif		
		/* Ethernet header length and indicator */ 
		eth_length = (uint8_t)(
						(uint8_t *)PARSER_GET_OUTER_IP_OFFSET_DEFAULT() - 
								(uint8_t *)PARSER_GET_ETH_OFFSET_DEFAULT()); 
				
		/* Remove L2 Header */	
		/* Note: The gross running sum of the frame becomes invalid 
		 * after calling this function.
		 * 
		 * TODO: is it important to update it for encryption???
		 * 
		 * For decryption in transport mode it is required to update 
		 * the running sum.
		 */
		l2_header_remove();
	}
			/*---------------------*/
			/* ipsec_frame_encrypt */
			/*---------------------*/
	
	/* 	5.	Save original FD[FLC], FD[FRC] (to stack) */
	orig_flc = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	orig_frc = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
	
	/* 	6.	Update the FD[FLC] with the flow context buffer address. */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS,(ipsec_handle + IPSEC_INTERNAL_PARMS_SIZE));	
	
	/* Clear FD[FRC], so DPOVRD takes no action */
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, 0);
	
	/* 	7.	FDMA store default frame command 
	 * (for closing the frame, updating the other FD fields) */
	return_val = fdma_store_default_frame_data();
	// TODO: check FDMA return status

	/* 	8.	Prepare AAP parameters in the Workspace memory. */
	/* 	8.1.	Use accelerator macros for storing parameters */
	
	
	/* 0 SW_FD Software Frame Descriptor = 0
	* FD should be read from the default workspace offset (0x00).
	*
	* 1 SW_FQDC Software Frame Queue Descriptor Context (FQDC) = 0
	* The ACC FQDC should be read from the default workspace offset (0x40).
	*
	* 2 SW_CMP Software Completion Frame Descriptor = 0
	* The Completion FD is written to the default workspace offset (0x00).
	* 
	* 3 USE_FLC_SP Use Flow Context Storage Profile = 1 
	* The Storage Profile (SP) is embedded as part of the 
	* flow context pointed to by the Flow Context field in the Frame Descriptor 
	* (FD) for this acceleration operation.
	* 
	* 8 OS_EX Ordering Scope Exclusive Phase.
	* 0 Indicates that the accelerator call is not made during the 
	* exclusive phase of an Ordering Scope.
	* 1 Indicates that the accelerator call is made during the 
	* exclusive phase of an Ordering Scope.
	*/
	//TODO: OS_EX currently set to 0, assuming exclusive mode only
	
	
	/* Get OSM status (ordering scope mode and levels) */
	osm_get_scope(&scope_status);

	/* If in Concurrent ordering scope, move to Exclusive 
	 * (increment scope ID). */ 
	if (scope_status.scope_mode == IPSEC_OSM_CONCURRENT) {
	    /* Move to exclusive */
	    osm_scope_transition_to_exclusive_with_increment_scope_id();
		/* Set OS_EX so AAP will do relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = 
				(IPSEC_AAP_USE_FLC_SP | IPSEC_AAP_OS_EX);
	} else {
		/* Call AAP without relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = IPSEC_AAP_USE_FLC_SP;
	}
	
	/* 	9.	Call the AAP */
	__e_hwacceli(AAP_SEC_ACCEL_ID);
	
	/* 	10.	SEC Doing Encryption */

			/*---------------------*/
			/* ipsec_frame_encrypt */
			/*---------------------*/
	
	/* Check if started in concurrent mode */
	if (scope_status.scope_mode == IPSEC_OSM_CONCURRENT) {
		/* The AAP already did OSM relinquished, so just register that */
		REGISTER_OSM_CONCURRENT;
	}

	/* Update the SPID of the new frame (SEC output) in the HW Context*/
	*((uint8_t *)HWC_SPID_ADDRESS) = sap1.output_spid;
	
	/* 	11.	FDMA present default frame command (open frame) */
	return_val = fdma_present_default_frame();
	// TODO: check for FDMA error
	
	/* 	12.	Read the SEC return status from the FD[FRC]. Use swap macro. */
	//*enc_status = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
	// TODO: which errors can happen in encryption?
	switch (LDPAA_FD_GET_FRC(HWC_FD_ADDRESS)) {
		case SEC_NO_ERROR:
			break;
		case SEC_SEQ_NUM_OVERFLOW: /** Sequence Number overflow */
			*enc_status |= IPSEC_SEQ_NUM_OVERFLOW;
			return_val = -1;
			break;
		case SEC_AR_LATE_PACKET:	/** Anti Replay Check: Late packet */
			*enc_status |= IPSEC_AR_LATE_PACKET;
			return_val = -1;
			break;
		case SEC_AR_REPLAY_PACKET:	/** Anti Replay Check: Replay packet */
			*enc_status |= IPSEC_AR_REPLAY_PACKET;
			return_val = -1;
			break;
		case SEC_ICV_COMPARE_FAIL:	/** ICV comparison failed */
			*enc_status |= IPSEC_ICV_COMPARE_FAIL;	
			return_val = -1;
			break;
		default:
			*enc_status |= IPSEC_GEN_ENCR_ERR;	
			return_val = -1;
	}
	
	/* 	13.	If encryption/encapsulation failed go to END (see below) */
	// TODO: check results
		
	/* 	14.	Get new running sum and byte count (encrypted/encapsulated frame) 
	 * from the FD[FLC] */
	/* The least significant 6 bytes of the 8-byte FLC in the enqueued FD 
	 * contain a 2-byte checksum and 4-byte encrypted/decrypted byte count.
	 * FLC[63:0] = { 16�b0, checksum[15:0], byte_count[31:0] } */
	//checksum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 2);
	
	/** Load 2 bytes with endian swap.
	 * The address loaded from memory is calculated as: _displ + _base.
	 * _displ - a word aligned constant value between 0-1020.
	 * _base - a variable containing the base address.
	 * If 'base' is a literal 0, the base address is considered as 0. */
	//#define LH_SWAP(_disp, _base) ((uint16_t)__lhbr((uint32_t)_disp, (void *)_base))
	checksum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 2, 0);

	
	//byte_count = LW_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 4);
	byte_count = LW_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 4, 0);

	/* 	15.	Update the gross running checksum in the Workspace parser results.*/
	// TODO: is it needed for encryption?
	
	/* 	16.	If L2 header existed in the original frame, add it back: */
	if (eth_length) {
		//TODO: debug info
#ifdef AIOP_VERIF
		if (ipsec_debug_buf_addr != NULL) {
			/* Write the debug info to external memory */
			cdma_write(
				(ipsec_debug_buf_addr + ipsec_debug_buf_offset), /* ext_address */
				&eth_header, /* ws_src */
				40); /* size */
			if (ipsec_debug_buf_offset <= (ipsec_debug_buf_size-64)) {
				ipsec_debug_buf_offset += 64;
			}
		}
#endif
		
		/* Note: The Ethertype was already updated before removing the 
		 * L2 header */
		return_val = fdma_insert_default_segment_data(
				0, /* uint16_t to_offset */
				eth_header, /* void	 *from_ws_src */
				eth_length, /* uint16_t insert_size */
				FDMA_REPLACE_SA_REPRESENT_BIT 
					/* uint32_t flags */
				);
		
		/* TODO: Re-run parser ??? */
		//		parse_result_generate_default(0);
		
		/* TODO: Update running sum ??? */
		//		pr->gross_running_sum = 0;
	}

		/*---------------------*/
		/* ipsec_frame_encrypt */
		/*---------------------*/

	/* 	17.	Restore the original FD[FLC], FD[FRC] (from stack). 
	 * No need for additional FDMA command. */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, orig_flc);	
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, orig_frc)	
	
	/* 	18.	Handle lifetime counters */
		/* 	18.1.	Read lifetime counters (CDMA) */
		/* 	18.2.	Add byte-count from SEC and one packet count. */
		/* 	18.4.	Update the kilobytes and/or packets lifetime counters 
		 * (STE increment + accumulate). */
	
	if (sap1.flags & 
			(IPSEC_FLG_LIFETIME_KB_CNTR_EN | IPSEC_FLG_LIFETIME_PKT_CNTR_EN)) {
		ste_inc_and_acc_counters(
			IPSEC_PACKET_COUNTER_ADDR,/* uint64_t counter_addr */
			byte_count,	/* uint32_t acc_value */
			/* uint32_t flags */
			(STE_MODE_COMPOUND_64_BIT_CNTR_SIZE |  
			STE_MODE_COMPOUND_64_BIT_ACC_SIZE |
			STE_MODE_COMPOUND_CNTR_SATURATE |
			STE_MODE_COMPOUND_ACC_SATURATE));
	} else if (sap1.flags & IPSEC_FLG_LIFETIME_KB_CNTR_EN) {
		ste_inc_counter(
				IPSEC_KB_COUNTER_ADDR,
				byte_count,
				(STE_MODE_SATURATE | STE_MODE_64_BIT_CNTR_SIZE));
	} else if (sap1.flags & IPSEC_FLG_LIFETIME_PKT_CNTR_EN) {
		ste_inc_counter(
				IPSEC_PACKET_COUNTER_ADDR,
				1,
				(STE_MODE_SATURATE | STE_MODE_64_BIT_CNTR_SIZE));
	}
	
	return_val = IPSEC_SUCCESS;	

encrypt_end:
	
	/* 	19.	END */
		
	/* 	19.1. Update the encryption status (enc_status) and return status. */

	/* Decrement the reference counter */
	return_val = cdma_refcount_decrement(ipsec_handle);
	// TODO: check CDMA return status
	
	/* 	19.3.	Return */
	return return_val;
} /* End of ipsec_frame_encrypt */

/**************************************************************************//**
* ipsec_frame_decrypt
*//****************************************************************************/
int ipsec_frame_decrypt(
		ipsec_handle_t ipsec_handle,
		uint32_t *dec_status
		)
{
	int return_val;
	uint8_t *last_etype_pointer;
	uint64_t eth_header[5]; /* Ethernet header place holder, 5*8=40 bytes */ 
	uint8_t eth_length = 0; /* Ethernet header length and indicator */ 
	uint64_t orig_flc; /* Original FLC */
	//uint64_t return_flc; /* SEC returned FLC */
	uint32_t orig_frc;
	uint16_t outer_material_length;
	//uint16_t running_sum;
	uint64_t *eth_pointer_default;
	uint32_t byte_count;
	uint16_t checksum;
	uint8_t dont_decrypt = 0;

	/* TMP debug code */
	uint32_t tmp_outer_ip_offset = (uint32_t)((uint8_t *)PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
	uint32_t tmp_eth_offset = (uint32_t)((uint8_t *)PARSER_GET_ETH_OFFSET_DEFAULT());
	uint32_t tmp_l5_offset = (uint32_t)((uint8_t *)PARSER_GET_L5_OFFSET_DEFAULT()); 
	
	struct ipsec_sa_params_part1 sap1; /* Parameters to read from ext buffer */
	struct scope_status_params scope_status;

	struct dpovrd_general dpovrd;
	struct   parse_result *pr =
				(struct parse_result *)HWC_PARSE_RES_ADDRESS;
	
	/* Increment the reference counter */
	cdma_refcount_increment(ipsec_handle);
	
	*dec_status = 0; /* Initialize */
	
	/* 	Inbound frame decryption and decapsulation */
	
	/* 	TODO: Currently supporting only Tunnel mode simplified flow */

	/* 	2.	Read relevant descriptor fields with CDMA. */
	cdma_read(
			&sap1, /* void *ws_dst */
			ipsec_handle, /* uint64_t ext_address */
			sizeof(sap1) /* uint16_t size */
			);

	/*---------------------*/
	/* ipsec_frame_decrypt */
	/*---------------------*/
	
	/* 	3.	Check that hard kilobyte/packet/seconds lifetime limits 
	 * have expired. If expired, return with error. go to END */
	// TODO
	/* The seconds lifetime status is checked in the params[status] 
	 * and the kilobyte/packet status is checked from the params[counters].
	 * This is done to avoid doing mutex lock for kilobyte/packet status */
	/* Seconds Lifetime */
	if (sap1.flags & IPSEC_FLG_LIFETIME_SEC_CNTR_EN) {
		if (sap1.status & IPSEC_STATUS_SOFT_SEC_EXPIRED) {
			*dec_status |= IPSEC_STATUS_SOFT_SEC_EXPIRED;
		}
		if (sap1.status & IPSEC_STATUS_HARD_SEC_EXPIRED) {
			*dec_status |= IPSEC_STATUS_HARD_SEC_EXPIRED;
			dont_decrypt = 1;
		}
	}
	
	/* KB lifetime counters */
	if (sap1.flags & IPSEC_FLG_LIFETIME_KB_CNTR_EN) {
		if (sap1.byte_counter >= sap1.soft_byte_limit) {
			*dec_status |= IPSEC_STATUS_SOFT_KB_EXPIRED;
		}
		if (sap1.byte_counter >= sap1.hard_byte_limit) {
			*dec_status |= IPSEC_STATUS_HARD_KB_EXPIRED;
			dont_decrypt = 1;
		}
	}
	
	/* Packets lifetime counters*/
	if (sap1.flags & IPSEC_FLG_LIFETIME_PKT_CNTR_EN) {

		if (sap1.packet_counter >= sap1.soft_packet_limit) {
			*dec_status |= IPSEC_STATUS_SOFT_PACKET_EXPIRED;
		}
		if (sap1.packet_counter >= sap1.hard_packet_limit) {
			*dec_status |= IPSEC_STATUS_HARD_PACKET_EXPIRED;
			dont_decrypt = 1;
		}
	}
	
	
	if (dont_decrypt) {
		return_val = IPSEC_ERROR; // TODO: TMP
		goto decrypt_end;
	}
	
			/*---------------------*/
			/* ipsec_frame_decrypt */
			/*---------------------*/
	
	/* 	4.	Identify if L2 header exist in the frame, 
	 * and if yes get the L2 header length. */
	if (PARSER_IS_ETH_MAC_DEFAULT()) { /* Check if Ethernet header exist */
		
		/* For tunnel mode, update the Ethertype field according to the 
		 * outer header (IPv4/Ipv6), since after SEC encryption
		 * the parser results are not valid any more */
		
		/* Get the pointer to last EtherType */
		last_etype_pointer = PARSER_GET_LAST_ETYPE_POINTER_DEFAULT();
		
		/* Update the Ethertype according to the outher IP header */
		// TODO. Currently this is always IPv4
		
		/* Ethernet header length and indicator */ 
		eth_length = (uint8_t)
				((uint8_t *)PARSER_GET_OUTER_IP_OFFSET_DEFAULT() - 
								(uint8_t *)PARSER_GET_ETH_OFFSET_DEFAULT()); 
					
		/* Transport Mode */
		if (!(sap1.flags & IPSEC_FLG_TUNNEL_MODE)) {
	
		/* Save Ethernet header. Note: no swap */
		/* up to 6 VLANs x 4 bytes + 14 regular bytes */
			eth_pointer_default = (uint64_t *)PARSER_GET_ETH_POINTER_DEFAULT();
			
			eth_header[0] = *(eth_pointer_default + 0);
			eth_header[1] = *(eth_pointer_default + 1);
			eth_header[2] = *(eth_pointer_default + 2);
			eth_header[3] = *(eth_pointer_default + 3);
			eth_header[4] = *(eth_pointer_default + 4);
			
			/* Ethernet header length and indicator */ 
					
			
			/* Remove L2 Header */	
			/* Note: The gross running sum of the frame becomes invalid 
			 * after calling this function. */ 
			 
			 l2_header_remove();
			
			// TODO: 
			/* For decryption in transport mode it is required to update 
			  * the running sum. */
			 			
		}
	}

			/*---------------------*/
			/* ipsec_frame_decrypt */
			/*---------------------*/

	/* 	5.	Save original FD[FLC], FD[FRC] (to stack) */
	orig_flc = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	orig_frc = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
	
	/* 	6.	Update the FD[FLC] with the flow context buffer address. */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS,(ipsec_handle + IPSEC_INTERNAL_PARMS_SIZE));	
	
	/* 7.	Update the FD[FRC] with SEC DPOVRD parameters */
	/* For transport mode: IP header length, Next header offset */
	/* For tunnel mode: 
	 * IP header length, Actual Outer IP Header Offset (AOIPHO), including L2 */
	if (sap1.flags & IPSEC_FLG_TUNNEL_MODE) {
		/* Prepare DPOVRD */
		/* 31 OVRD
		 * 30-20 Reserved
		 * 19-12 AOIPHO: Actual Outer IP Header Offset. 
		 * 		AOIPHO indicates the number of bytes of material in the 
		 * 		Input Frame prior to the actual Outer IP Header.
		 * 11-0 Outer IP Header Material Length 
		 * 		Length for the Outer IP Header Material (in bytes). 
		 * 		This field indicates the total length of the material that 
		 * 		includes the Outer IP Header, up to but not including the 
		 * 		ESP Header.
		*/
		outer_material_length = (uint16_t) // TODO: verify this 
			((uint32_t)((uint8_t *)PARSER_GET_L5_OFFSET_DEFAULT()) 
					- (uint32_t)((uint8_t *)PARSER_GET_ETH_OFFSET_DEFAULT())); 
		
		// If this is incorrect, get the IP length from the header wit
		// PARSER_GET_OUTER_IP_POINTER_DEFAULT()
				
		dpovrd.tunnel_decap.word = 
				IPSEC_DPOVRD_OVRD |
				(eth_length<<12) | /* AOIPHO */
				outer_material_length; /* Outer IP Header Material Length */
		// TODO:  outer_material_length in case of UDP??
				
		LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, *((uint32_t *)(&dpovrd)));

	} else { /* Transport Mode */
		// TODO
	}
	
	/* 	8.	FDMA store default frame command 
	 * (for closing the frame, updating the other FD fields) */
	return_val = fdma_store_default_frame_data();
	
	/* 	9.	Prepare AAP parameters in the Workspace memory. */
	
	/* 0 SW_FD Software Frame Descriptor = 0 */
	/* 1 SW_FQDC Software Frame Queue Descriptor Context (FQDC) = 0 */
	/* 2 SW_CMP Software Completion Frame Descriptor = 0 */
	/* 3 USE_FLC_SP Use Flow Context Storage Profile = 1 */ 
	/* 8 OS_EX Ordering Scope Exclusive Phase.
	* 0 Indicates that the accelerator call is not made during the 
	* exclusive phase of an Ordering Scope.
	* 1 Indicates that the accelerator call is made during the 
	* exclusive phase of an Ordering Scope.
	*/
	
	/* Get OSM status (ordering scope mode and levels) */
	osm_get_scope(&scope_status);

	/* If in Concurrent ordering scope, move to Exclusive 
	 * (increment scope ID). */ 
	if (scope_status.scope_mode == IPSEC_OSM_CONCURRENT) {
	    /* Move to exclusive */
	    osm_scope_transition_to_exclusive_with_increment_scope_id();
		/* Set OS_EX so AAP will do relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = 
				(IPSEC_AAP_USE_FLC_SP | IPSEC_AAP_OS_EX);
	} else {
		/* Call AAP without relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = IPSEC_AAP_USE_FLC_SP;
	}
	
		
	/* 	10.	Call the AAP */
	__e_hwacceli(AAP_SEC_ACCEL_ID);

	/* 	11.	SEC Doing Decryption */

	/* Check if started in concurrent mode */
	if (scope_status.scope_mode == IPSEC_OSM_CONCURRENT) {
		/* The AAP already did OSM relinquished, so just register that */
		REGISTER_OSM_CONCURRENT;
	}

	/* Update the SPID of the new frame (SEC output) in the HW Context*/
	*((uint8_t *)HWC_SPID_ADDRESS) = sap1.output_spid;
	
	// TODO: default segment size change (TBD) */

	/* 	12.	FDMA present default frame command */ 
	return_val = fdma_present_default_frame();

	/* 	13.	Read the SEC return status from the FD[FRC]. Use swap macro. */
	// TODO: which errors can happen in decryption?
	switch (LDPAA_FD_GET_FRC(HWC_FD_ADDRESS)) {
		case SEC_NO_ERROR:
			break;
		case SEC_SEQ_NUM_OVERFLOW: /** Sequence Number overflow */
			*dec_status |= IPSEC_SEQ_NUM_OVERFLOW;
			return_val = -1;
			break;
		case SEC_AR_LATE_PACKET:	/** Anti Replay Check: Late packet */
			*dec_status |= IPSEC_AR_LATE_PACKET;
			return_val = -1;
			break;
		case SEC_AR_REPLAY_PACKET:	/** Anti Replay Check: Replay packet */
			*dec_status |= IPSEC_AR_REPLAY_PACKET;
			return_val = -1;
			break;
		case SEC_ICV_COMPARE_FAIL:	/** ICV comparison failed */
			*dec_status |= IPSEC_ICV_COMPARE_FAIL;	
			return_val = -1;
			break;
		default:
			*dec_status |= IPSEC_GEN_ENCR_ERR;	
			return_val = -1;
	}

	/* 	14.	If encryption/encapsulation failed go to END (see below) */
	// TODO: check results
		
	/* 	15.	Get new running sum and byte count (encrypted/encapsulated frame) 
	 * from the FD[FLC] */
	
	/* From Martin Dorr 27-Mar-2014: 
	 * A 32-bit byte count is stored in the LS portion of the FLC in LE format.
	 * A 2-byte checksum is stored starting at offset 4 relative to the 
	 * beginning of the FLC.
	 * FLC[63:0] = { 16�b0, checksum[15:0], byte_count[31:0] }
	*/
	// TODO (still not implemented in the simulator)
	//return_flc = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	//checksum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 2);
	//byte_count = LW_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 4);
	checksum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 2, 0);
	byte_count = LW_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 4, 0);
	
	/* 	16.	Update the gross running checksum in the Workspace parser results.*/
	//pr->gross_running_sum = 
	//		(uint16_t)((return_flc & IPSEC_RETURN_FLC_CHECKSUM_BE_MASK)
	//				>>IPSEC_RETURN_FLC_CHECKSUM_BE_SHIFT);
	pr->gross_running_sum = checksum;
			
	// TODO: handle in transport mode
	
			/*---------------------*/
			/* ipsec_frame_decrypt */
			/*---------------------*/

	/* 	17.	Run parser and check for errors. */
	return_val = parse_result_generate_default(PARSER_VALIDATE_L3_L4_CHECKSUM);
	// TODO: mask out some parser error bits, in case there is no L4 etc.
	// TODO: special handling in case of fragments
	
	/* 	18.	If validity check failed, go to END, return with error. */
	// TODO
	
	/* 	19.	Restore the original FD[FLC], FD[FRC] (from stack) */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, orig_flc);	
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, orig_frc)	

	/* 	20.	Handle lifetime counters */
	/* 	20.1.	Read lifetime counters (CDMA) */
	/* 	20.2.	Add byte-count from SEC and one packet count. */
	/* 	20.3.	Calculate locally if lifetime counters crossed the limits. 
	 * If yes set flag in the descriptor statistics (CDMA write). */
	/* 	20.4.	Update the kilobytes and/or packets lifetime counters 
	 * (STE increment + accumulate). */
	if (sap1.flags & 
			(IPSEC_FLG_LIFETIME_KB_CNTR_EN | IPSEC_FLG_LIFETIME_PKT_CNTR_EN)) {
		ste_inc_and_acc_counters(
			IPSEC_PACKET_COUNTER_ADDR,/* uint64_t counter_addr */
			byte_count,	/* uint32_t acc_value */
			/* uint32_t flags */
			(STE_MODE_COMPOUND_64_BIT_CNTR_SIZE |  
			STE_MODE_COMPOUND_64_BIT_ACC_SIZE |
			STE_MODE_COMPOUND_CNTR_SATURATE |
			STE_MODE_COMPOUND_ACC_SATURATE));
	} else if (sap1.flags & IPSEC_FLG_LIFETIME_KB_CNTR_EN) {
		ste_inc_counter(
				IPSEC_KB_COUNTER_ADDR,
				byte_count,
				(STE_MODE_SATURATE | STE_MODE_64_BIT_CNTR_SIZE));
	} else if (sap1.flags & IPSEC_FLG_LIFETIME_PKT_CNTR_EN) {
		ste_inc_counter(
				IPSEC_PACKET_COUNTER_ADDR,
				1,
				(STE_MODE_SATURATE | STE_MODE_64_BIT_CNTR_SIZE));
	}
	
	return_val = IPSEC_SUCCESS;	

decrypt_end:
	
	/* 	21.	END */
	/* 	21.1. Update the encryption status (enc_status) and return status. */
	/* 	21.2. If started as Concurrent ordering scope, 
	 *  move from Exclusive to Concurrent 
	 *  (if AAP does that, only register through OSM functions). */
	// TODO
	
	/* Check if started in concurrent mode */
	if (scope_status.scope_mode == IPSEC_OSM_CONCURRENT) {
		/* Move to Concurrent */
		osm_scope_relinquish_exclusivity();
	}

	/* Decrement the reference counter */
	return_val = cdma_refcount_decrement(ipsec_handle);
	// TODO: check CDMA return status
	
	/* Return */
	return return_val;
} /* End of ipsec_frame_decrypt */


/**************************************************************************//**
	ipsec_get_lifetime_stats
*//****************************************************************************/
int ipsec_get_lifetime_stats(
		ipsec_handle_t ipsec_handle,
		uint64_t *kilobytes,
		uint64_t *packets,
		uint32_t *sec)
{
	
	int return_val;
	uint64_t current_timestamp;
	
	/* Note: this struct must be equal to the head of ipsec_sa_params_part1 */
	struct counters_and_timestamp {
		uint64_t packet_counter; /*	Packets counter, 8B */
		uint64_t byte_counter; /* Encrypted/decrypted bytes counter, 8B */
		uint64_t timestamp; /* TMAN timestamp in micro-seconds, 8 Bytes */
	} ctrs;
	
	/* Increment the reference counter */
	cdma_refcount_increment(ipsec_handle);
	// TODO: check CDMA return status
	
	/* Flush all the counter updates that are pending in the 
	 * statistics engine request queue. */
	ste_barrier();

	/* 	Read relevant descriptor fields with CDMA. */
	cdma_read(
			&ctrs, /* void *ws_dst */
			ipsec_handle, /* uint64_t ext_address */
			sizeof(ctrs) /* uint16_t size */
			);
	
	*packets = ctrs.packet_counter;
	*kilobytes =  ctrs.byte_counter;
	
	/* Get current timestamp from TMAN (in micro-seconds)*/
	tman_get_timestamp(&current_timestamp);

	/* Calculate elapsed time in seconds */
	/* Do shift 20, since 2^20 = 1,048,576 */
	if (current_timestamp >= ctrs.timestamp) { /* No roll-over */
		*sec = (uint32_t)((current_timestamp - ctrs.timestamp)>>20);
	} else { /* Roll-over */
		*sec = (uint32_t)(
				(current_timestamp + 
						(IPSEC_MAX_TIMESTAMP - ctrs.timestamp) + 1)>>20);
	}

	/* Decrement the reference counter */
	return_val = cdma_refcount_decrement(ipsec_handle);
	// TODO: check CDMA return status
	
	return IPSEC_SUCCESS;
	
} /* End of ipsec_get_lifetime_stats */

/**************************************************************************//**
	ipsec_decr_lifetime_counters
*//****************************************************************************/
int ipsec_decr_lifetime_counters(
		ipsec_handle_t ipsec_handle,
		uint32_t kilobytes_decr_val,
		uint32_t packets_decr_val
		)
{
	/* Note: there is no check of counters enable, nor current value.
	 * Assuming that it is only called appropriately by the upper layer */
	int return_val;

	/* Increment the reference counter */
	cdma_refcount_increment(ipsec_handle);
	
	/* Flush all the counter updates that are pending in the 
	 * statistics engine request queue. */
	ste_barrier();
	
	if (kilobytes_decr_val) {
		ste_dec_counter(
				IPSEC_KB_COUNTER_ADDR,
				kilobytes_decr_val,
				(STE_MODE_SATURATE | STE_MODE_64_BIT_CNTR_SIZE));
	}
	
	if (packets_decr_val) {
		ste_dec_counter(
				IPSEC_PACKET_COUNTER_ADDR,
				packets_decr_val,
				(STE_MODE_SATURATE | STE_MODE_64_BIT_CNTR_SIZE));
	}	
	
	/* Decrement the reference counter */
	return_val = cdma_refcount_decrement(ipsec_handle);
	// TODO: check CDMA return status
	
	return IPSEC_SUCCESS;	
} /* End of ipsec_decr_lifetime_counters */

/**************************************************************************//**
	ipsec_get_seq_num
*//****************************************************************************/
int ipsec_get_seq_num(
		ipsec_handle_t ipsec_handle,
		uint32_t *sequence_number,
		uint32_t *extended_sequence_number,
		uint32_t anti_replay_bitmap[4])
{
	
	int return_val;
	struct ipsec_decap_pdb decap_pdb;
	
	/* Increment the reference counter */
	cdma_refcount_increment(ipsec_handle);
	
	/* 	Read the PDB from the descriptor with CDMA. */
	cdma_read(
			&decap_pdb, /* void *ws_dst */
			IPSEC_PDB_ADDR, /* uint64_t ext_address */
			sizeof(decap_pdb) /* uint16_t size */
	);
	
	/* Return swapped values (little to big endian conversion) */
	*extended_sequence_number = LW_SWAP(0,&decap_pdb.seq_num_ext_hi);
	*sequence_number = LW_SWAP(0,&decap_pdb.seq_num);

	switch (decap_pdb.options & IPSEC_DECAP_PDB_ARS_MASK) {
		case IPSEC_DEC_OPTS_ARSNONE:
			anti_replay_bitmap[0] = 0x0;
			anti_replay_bitmap[1] = 0x0;
			anti_replay_bitmap[2] = 0x0;
			anti_replay_bitmap[3] = 0x0;
			break;
		case IPSEC_DEC_OPTS_ARS32:
			anti_replay_bitmap[0] = LW_SWAP(0,&decap_pdb.anti_replay[0]);
			anti_replay_bitmap[1] = 0x0;
			anti_replay_bitmap[2] = 0x0;
			anti_replay_bitmap[3] = 0x0;
			break;
		case IPSEC_DEC_OPTS_ARS64:
			anti_replay_bitmap[0] = LW_SWAP(0,&decap_pdb.anti_replay[0]);
			anti_replay_bitmap[1] = LW_SWAP(0,&decap_pdb.anti_replay[1]);
			anti_replay_bitmap[2] = 0x0;
			anti_replay_bitmap[3] = 0x0;
			break;		
		case IPSEC_DEC_OPTS_ARS128:	
			anti_replay_bitmap[0] = LW_SWAP(0,&decap_pdb.anti_replay[0]);
			anti_replay_bitmap[1] = LW_SWAP(0,&decap_pdb.anti_replay[1]);
			anti_replay_bitmap[2] = LW_SWAP(0,&decap_pdb.anti_replay[2]);
			anti_replay_bitmap[3] = LW_SWAP(0,&decap_pdb.anti_replay[3]);
			break;
		default:
			anti_replay_bitmap[0] = 0x0;
			anti_replay_bitmap[1] = 0x0;
			anti_replay_bitmap[2] = 0x0;
			anti_replay_bitmap[3] = 0x0;	
	}
		
	/* Derement the reference counter */
	return_val = cdma_refcount_decrement(ipsec_handle);
	// TODO: check CDMA return status
	
	return IPSEC_SUCCESS;	

} /* End of ipsec_get_seq_num */


/**************************************************************************/

/** @} */ /* end of FSL_IPSEC_Functions */

/** @} */ /* end of FSL_IPSEC */
/** @} */ /* end of NETF */



/**************************************************************************//**
@File		dpni_drv.h

@Description	Data Path Network Interface API
*//***************************************************************************/
#ifndef __DPNI_DRV_H
#define __DPNI_DRV_H

#include "common/types.h"
#include "dplib/fsl_dpni.h"
#include "dplib/fsl_ldpaa.h"

/**************************************************************************//**
@Group		grp_dplib_aiop	DPLIB

@Description	Contains initialization APIs and runtime control APIs for DPNI

@{
*//***************************************************************************/
/**************************************************************************//**
@Group		grp_dpni_aiop	DPNI (AIOP Data Path Network Interface API)

@Description	Contains initialization APIs and runtime control APIs for DPNI

@{
*//***************************************************************************/

/* TODO - move to soc files */
#define SOC_MAX_NUM_OF_DPNI		128

#define DPNI_DRV_MAX_NUM_FLOWS		8

/**************************************************************************//**
@Group	DPNI_DRV_STATUS
@{
*//***************************************************************************/
/** MTU was crossed for DPNI driver send function */
#define	DPNI_DRV_MTU_ERR	(DPNI_DRV_MODULE_STATUS_ID | 0x1)
/* @} */

typedef uint64_t	dpni_drv_app_arg_t;

/* TODO: need to define stats */
struct dpni_stats {
	int num_pkts;
};

/**************************************************************************//**
@Description	Application Receive callback

		User provides this function. Driver invokes it when it gets a
		frame received on this interface.

@Param[in]	arg     argument for application callback.

@Return	OK on success; error code, otherwise.
*//***************************************************************************/
typedef void /*__noreturn*/ (rx_cb_t) (dpni_drv_app_arg_t arg);


/**************************************************************************//**
@Function	dpni_drv_enable

@Description	Enable a NI_ID referenced by ni_id. Allows sending and
		receiving frames.

@Param[in]	ni_id   The Network Interface ID

@Return	OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_enable(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_disable

@Description	Disables a NI_ID referenced by ni_id. Disallows sending and
		receiving frames

@Param[in]	ni_id	The Network Interface ID

@Return	OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_disable(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_get_attrib

@Description	Returns the value of the NI_ID attribute specified with the
	attrib argument. See the dpni_ni_attrib enum for the list of supported
	attributes.

@Param[in]	ni_id   The Network Interface ID
@Param[in]	attrib  The NI attribute to be returned

@Return        The value of the attribute, negative value in case of an error
*//***************************************************************************/
int dpni_drv_get_attrib(uint16_t ni_id, int attrib);

/**************************************************************************//**
@Function	dpni_drv_get_stats

@Description	Retrieve NI statistics

@Param[in]	ni_id - Network Interface ID
@Param[out]	stats - Statics

@Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_get_stats(uint16_t	ni_id, struct dpni_stats *stats);

/**************************************************************************//**
@Function	dpni_drv_reset_stats

@Description	Reset NI statistics.

@Param[in]	ni_id - Network Interface ID

@Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_reset_stats(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_set_rx_tc

@Description	Set RX TC settings

@Param[in]	dpni - Network Interface ID
@Param[in]	tc_id - Traffic class id
@Param[out]	cfg - DPNI RX Traffic class parameters

@Return	'0' on Success; error code otherwise.
*//***************************************************************************/
int dpni_set_rx_tc(struct dpni *dpni,
		   uint8_t tc_id,
		   const struct dpni_rx_tc_cfg *cfg);

/**************************************************************************//**
@Function	dpni_drv_register_discard_rx_cb

@Description	register a default receive callback functions.
		The default callback function will be called when a frame is
		received at a NI_ID that does not have a registered callback.
		Packet arrives on an NI, there is registered handler. AIOP SL
		invokes the registered callback.  wing logic is implemented:
		Packet arrives on an NI, there is no registered handler.
		AIOP SL checks if
		there is a default handler and invokes the registered callback
		for the default handler.  Packet arrives on an NI, there is no
		registered handler. Also, there is no registered default
		handler. AIOP SL drops the packet.

@Param[in]	dpio    TODO
@Param[in]	dpsp    TODO
@Param[in]	cb      TODO
@Param[in]	arg     TODO

@Return	OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_register_discard_rx_cb(
		fsl_handle_t		dpio,
		fsl_handle_t		dpsp,
		rx_cb_t			*cb,
		dpni_drv_app_arg_t	arg);

/**************************************************************************//**
@Function	dpni_get_num_of_ni

@Description	Returns the number of NI_ISs in the system.  Called by the AIOP
		applications to learn the maximum number of available network
		interfaces.

@Return	Number of NI_IDs in the system
*//***************************************************************************/
int dpni_get_num_of_ni(void);

#endif /* __DPNI_DRV_H */
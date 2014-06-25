/**************************************************************************//**
 @File		fsl_l2.h

 @Description	This file contains the AIOP Header Modification - L2 HM API

 @Cautions	Please note that the parse results must be updated before
 calling functions defined in this file.

 Copyright 2013 Freescale Semiconductor, Inc.

 *//***************************************************************************/

#ifndef __FSL_L2_H
#define __FSL_L2_H

#include "common/types.h"

/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
 *//***************************************************************************/
/**************************************************************************//**
 @Group		AIOP_L2 L2

 @Description	AIOP L2 related API

 @{
 *//***************************************************************************/
/**************************************************************************//**
 @Group		AIOP_L2_HM L2 Header Modifications

 @Description	AIOP L2 related header modifications API

 @{
 *//***************************************************************************/

/** No VLAN exists.*/
#define NO_VLAN_ERROR				(HM_MODULE_STATUS_ID + 0x0400)

/**************************************************************************//**
 @Group		FSL_HM_L2_Functions HM L2 related functions

 @Description	L2 related Header Modification functions

 @{
 *//***************************************************************************/

/*************************************************************************//**
 @Function	l2_header_remove

 @Description	Remove Ethernet/802.3 MAC header.

 If VLAN tags are present they are also removed.
 If MPLS header exists, it is also removed.

 The gross running sum of the frame becomes invalid after calling
 this function.

 Implicit input parameters in task defaults: frame handle,
 segment handle, parser_profile_id, parser_starting_hxs.
 Implicitly updated values in task defaults: segment length,
 segment address, parser_starting_hxs.

 @Return		None.

 @Cautions
 - This function assumes that the ethernet header exists
 in the packet.
 - The parse results must be updated before calling this
 operation.
 - In this function, the task yields.

 *//***************************************************************************/
void l2_header_remove(void);

/*************************************************************************//**
 @Function	l2_vlan_header_remove

 @Description	VLAN Header removal. Remove the stacked QTags if exists.
 (Unlimited stacked QTags).

 This function assumes an Ethernet header is present.

 The parse results are updated automatically at the end of this
 operation.

 The gross running sum of the frame becomes invalid after calling
 this function.

 Implicit input parameters in task defaults: frame handle,
 segment handle, parser_profile_id, parser_starting_hxs.
 Implicitly updated values in task defaults: segment length,
 segment address,parser_starting_hxs.

 @Return
 0 - Success.
 HM_ERROR_NO_VLAN - There was no VLAN in the frame.
 HM_ERROR_NO_IP_HDR - There is no IP header after the stacked QTags.

 @Cautions	The parse results must be updated before calling this operation.
 In this function, the task yields

 *//***************************************************************************/
int32_t l2_vlan_header_remove(void);

/*************************************************************************//**
 @Function	l2_set_vlan_vid

 @Description	Set the 802.1q outer VLAN id.

 @Param[in]	vlan_vid - VLAN Identifier (VID).

 @Return		Success or Failure (There was no VLAN in the frame).

 *//***************************************************************************/
int32_t l2_set_vlan_vid(uint16_t vlan_vid);

/*************************************************************************//**
 @Function	l2_set_vlan_pcp

 @Description	Set the 802.1q priority in the outer VLAN.

 @Param[in]	vlan_pcp - VLAN Priority Code Point (PCP).

 @Return		Success or Failure (There was no VLAN in the frame).

 *//***************************************************************************/
int32_t l2_set_vlan_pcp(uint8_t vlan_pcp);

/*************************************************************************//**
 @Function	l2_set_dl_src

 @Description	Replace the Ethernet source address

 @Param[in]	src_addr - Pointer to the new Ethernet source address.

 @Return		None.

 @Cautions	None.
 *//***************************************************************************/
void l2_set_dl_src(uint8_t *src_addr);

/*************************************************************************//**
 @Function	l2_set_dl_dst

 @Description	Replace the Ethernet destination address

 @Param[in]	dst_addr - Pointer to the new Ethernet destination address.

 @Return		None.

 @Cautions	None.
 *//***************************************************************************/
void l2_set_dl_dst(uint8_t *dst_addr);

/*************************************************************************//**
 @Function	l2_push_vlan

 @Description	Push a new outer VLAN tag.\n

 This function assumes the presence of an Ethernet header.

 The parse results are updated automatically at the end of
 this operation.

 The gross running sum of the frame becomes invalid after calling
 this function.

 Implicit input parameters in Task Defaults: frame handle,
 segment handle, segment address.
 Implicit output parameters in Task Defaults: parser_starting_hxs


 @Param[in]	ethertype -  Indicates the Ethertype of the new tag.

 @Return		None.

 @Cautions	The parse results must be updated before calling this operation.
 If an ethernet header is present, it is assumed to be located at
 the beginning of the segment (offset 0 from segment address).
 If there is no ethernet header, the vlan is inserted at the
 beginning of the segment.
 *//***************************************************************************/
void l2_push_vlan(uint16_t ethertype);

/*************************************************************************//**
 @Function	l2_push_and_set_vlan

 @Description	Push and set a new outer VLAN tag.\n

 This function assumes the presence of an Ethernet header.

 The parse results are updated automatically at the end of
 this operation.

 The gross running sum of the frame becomes invalid after calling
 this function.

 Implicit input parameters in Task Defaults: frame handle,
 segment handle, segment address.
 Implicit output parameters in Task Defaults: parser_starting_hxs


 @Param[in]	vlan_tag -  Indicates the vlan tag value.

 @Return		None.

 @Cautions	The parse results must be updated before calling this operation.
 If an ethernet header is present, it is assumed to be located at
 the beginning of the segment (offset 0 from segment address).
 If there is no ethernet header, the vlan is inserted at the
 beginning of the segment.
 *//***************************************************************************/
void l2_push_and_set_vlan(uint32_t vlan_tag);

/*************************************************************************//**
 @Function	l2_pop_vlan

 @Description	Pop the outer VLAN tag.
 The parse results are updated automatically at the end of
 this operation.

 The gross running sum of the frame becomes invalid after calling
 this function.

 Implicit input parameters in Task Defaults: frame handle,
 segment handle, segment address.

 @Return		Success or Failure (There was no VLAN in the frame).

 @Cautions	The parse results must be updated before calling this operation.
 This function assumes the presence of the ethernet header.
 *//***************************************************************************/
int32_t l2_pop_vlan(void);

/*************************************************************************//**
 @Function	l2_arp_response

 @Description	Creates an ARP Response frame from an ARP Request frame. This
		function Updates both ETH and ARP header fields to create an ARP
		Response frame.

		The function assumes the ARP Request source frame is the default
		frame, and that the parse results are updated.

		The gross running sum of the frame becomes invalid after calling
		this function.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

 @Return	None.

 @Cautions
 	 	 - The parse results must be updated before calling this
 	 	 operation.
 	 	 - This function assumes the presence of the ETH header and ARP
 	 	 Request header.
 *//***************************************************************************/
void l2_arp_response();

/* Update ETH fields (MAC source, MAC destination, Ethertype).
 * Assuming a default segment is presented with parse results updated.
 * TODO - add documentation*/
/*************************************************************************//**
 @Function	l2_set_hw_src_dst

 @Description	Sets ETH HW source and destination addresses. The function
		assumes the ETH source frame is the default frame, and that the
		parse results are updated.

		The gross running sum of the frame becomes invalid after calling
		this function.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	target_hw_addr -  Target MAC address.

 @Return	None.

 @Cautions
 	 	 - The parse results must be updated before calling this
 	 	 operation.
 	 	 - This function assumes the presence of the ETH header.
 *//***************************************************************************/
void l2_set_hw_src_dst(uint8_t *target_hw_addr);

/* @} end of group FSL_HM_L2_Functions */
/* @} end of group AIOP_L2_HM */
/* @} end of group AIOP_L2 */
/* @} end of group NETF */

#endif /* __FSL_L2_H */

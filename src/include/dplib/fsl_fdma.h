/**************************************************************************//**
@File		fsl_fdma.h

@Description	This file contains the AIOP SW FDMA API

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __FSL_FDMA_H_
#define __FSL_FDMA_H_


#include "general.h"


/* extern uint8_t HWC_PRC_ADDR[]; */


/**************************************************************************//**
 @Group		FSL_AIOP_FDMA

 @Description	FSL AIOP FDMA macros and functions

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		FDMA_General_Definitions FDMA General Definitions

 @Description	FDMA General Definitions

 @{
*//***************************************************************************/
	/** SW annotation segment handle */
#define FDMA_PTA_SEG_HANDLE	0xF
	/** HW annotation segment handle */
#define FDMA_ASA_SEG_HANDLE	0xE
	/** Segment reference bit mask   */
#define FDMA_SEG_REF_MASK	0x1

/* @} end of group FDMA_General_Definitions */


/**************************************************************************//**
 @Group		FDMA_Enumerations FDMA Enumerations

 @Description	FDMA Enumerations

 @{
*//***************************************************************************/

/**************************************************************************//**
 @enum fdma_hw_errors

 @Description	AIOP FDMA Hardware Error codes.

 @{
*//***************************************************************************/
enum fdma_hw_errors {
		/** Success. */
	FDMA_SUCCESS = SUCCESS,
		/** Enqueue failed due to QMan enqueue rejection. */
	FDMA_ENQUEUE_FAILED_ERR = 0x1,
		/** Failed due to buffer pool depletion. */
	FDMA_BUFFER_POOL_DEPLETION_ERR = 0x2,
		/** Unable to split frame, split size is larger than frame
		 * size or no SF bit found. */
	FDMA_UNABLE_TO_SPLIT_ERR = 0x4,
		/** Unable to trim frame to concatenate, trim size is larger
		 * than frame size. */
	FDMA_UNABLE_TO_TRIM_ERR = 0x6,
		/** Frame Store failed, single buffer frame full and Storage
		 * Profile FF is set to 10. */
	FDMA_FRAME_STORE_ERR = 0x7,
		/** Unable to fulfill specified segment presentation size. */
	FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR = 0x8,
		/** ASAPO value beyond ASAL in received FD. No ASA presentation
		 * possible. */
	FDMA_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR = 0x9,
		/** Unable to fulfill specified ASAPS on an initial frame
		 * presentation. Only ASAL amount of annotation was presented.*/
	FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR = 0xA,
		/** Received non-zero FD[ERR] field from Work Scheduler. */
	FDMA_FD_ERR = 0xE,
		/** Frame Handle depletion (max of 6). */
	FDMA_FRAME_HANDLE_DEPLETION_ERR = 0x80,
		/** Invalid Frame Handle. */
	FDMA_INVALID_FRAME_HANDLE_ERR = 0x81,
		/** Segment Handle depletion (max of 8). */
	FDMA_SEGMENT_HANDLE_DEPLETION_ERR = 0x82,
		/** Invalid Segment Handle. */
	FDMA_INVALID_SEGMENT_HANDLE_ERR = 0x83,
		/** Invalid DMA command arguments. */
	FDMA_INVALID_DMA_COMMAND_ARGS_ERR = 0x86,
		/** Invalid DMA command. */
	FDMA_INVALID_DMA_COMMAND_ERR = 0x87,
		/** Internal memory ECC uncorrected ECC error. */
	FDMA_INTERNAL_MEMORY_ECC_ERR = 0xA0,
		/** Workspace memory read Error. */
	FDMA_WORKSPACE_MEMORY_READ_ERR = 0xA1,
		/** Workspace memory write Error. */
	FDMA_WORKSPACE_MEMORY_WRITE_ERR = 0xA2,
		/** System memory read error (permission or ECC). */
	FDMA_SYSTEM_MEMORY_READ_ERR = 0xA3,
		/** System memory write error (permission or ECC). */
	FDMA_SYSTEM_MEMORY_WRITE_ERR = 0xA4,
		/** QMan enqueue error (access violation). */
	FDMA_QMAN_ENQUEUE_ERR = 0xA5,
		/** Frame structural error (invalid S/G bits settings, hop
		 * limit). */
	FDMA_FRAME_STRUCTURAL_ERR = 0xA6,
		/** FDMA Internal error, SRU depletion. */
	FDMA_INTERNAL_ERR = 0xA7,
		/** Storage Profile ICID does not match frame ICID and Storage
		 * Profile BS=1 error. */
	FDMA_SPID_ICID_ERR = 0xA8
};

/* @} end of enum fdma_hw_errors */

/**************************************************************************//**
 @enum fdma_sw_errors

 @Description	AIOP FDMA Software Error codes.

 @{
*//***************************************************************************/
enum fdma_sw_errors {
		/** The segment handle does not represent a Data segment. */
	FDMA_NO_DATA_SEGMENT_HANDLE = 0x70,
		/** Invalid PTA address (\ref PRC_PTA_NOT_LOADED_ADDRESS). */
	FDMA_INVALID_PTA_ADDRESS = 0x71
};

/* @} end of enum fdma_sw_errors */

/**************************************************************************//**
 @enum fdma_st_options

 @Description	AIOP FDMA segment type options.

 @{
*//***************************************************************************/
enum fdma_st_options {
	/** Data Segment. */
	FDMA_ST_DATA_SEGMENT_BIT =	0x00000000,
	/** PTA Segment. */
	FDMA_ST_PTA_SEGMENT_BIT =	0x00000400,
	/** ASA Segment. */
	FDMA_ST_ASA_SEGMENT_BIT =	0x00000800
};

/* @} end of enum fdma_st_options */

/**************************************************************************//**
 @enum fdma_cfa_options

 @Description	AIOP FDMA copy frame annotations options.

 @{
*//***************************************************************************/
enum fdma_cfa_options {
	/** Do not copy annotations. */
	FDMA_CFA_NO_COPY_BIT =	0x00000000,
	/** Copy ASA. */
	FDMA_CFA_ASA_COPY_BIT =	0x00002000,
	/** Copy PTA. */
	FDMA_CFA_PTA_COPY_BIT =	0x00004000,
	/** Copy ASA + PTA. */
	FDMA_CFA_COPY_BIT =	0x00006000
};

/* @} end of enum fdma_cfa_options */

/**************************************************************************//**
 @enum fdma_split_psa_options

 @Description	AIOP FDMA Split command Post split action.

 @{
*//***************************************************************************/
enum fdma_split_psa_options {
	/** Do not present segment from the split frame,
	 * keep split working frame open. */
	FDMA_SPLIT_PSA_NO_PRESENT_BIT =	0x00000000,
	/** Present segment from the split frame,
	 * keep split working frame open. */
	FDMA_SPLIT_PSA_PRESENT_BIT =	0x00000400,
	/** Do not present, close split working frame. */
	FDMA_SPLIT_PSA_CLOSE_FRAME_BIT = 0x00000800
};

/* @} end of enum fdma_split_psa_options */

/**************************************************************************//**
 @enum fdma_enqueue_tc_options

 @Description	AIOP FDMA Enqueue Terminate Control options.

 @{
*//***************************************************************************/
enum fdma_enqueue_tc_options {
	/** Return after enqueue	*/
	FDMA_EN_TC_RET_BITS =	0x0000,
	/** Terminate: this command will trigger the Terminate task
	 * command right after the enqueue. If the enqueue failed, the
	 * frame will be discarded.	*/
	FDMA_EN_TC_TERM_BITS = 0x0400,
	/** Conditional Terminate: trigger the Terminate task command
	 * only if the enqueue succeeded. If the enqueue failed, the frame
	 * handle is not released and the command returns with an error
	 * code.	*/
	FDMA_EN_TC_CONDTERM_BITS =	0x0800
};

/* @} end of enum fdma_enqueue_tc_options */

/**************************************************************************//**
 @enum fdma_replace_sa_options

 @Description	AIOP FDMA Replace Segment Action options.

		When this command is invoked on annotation segments (PTA or ASA)
		they always remain open (i.e. setting
		\ref FDMA_REPLACE_SA_CLOSE_BIT has no effect and behaves the
		same as setting \ref FDMA_REPLACE_SA_OPEN_BIT).
 @{
*//***************************************************************************/
enum fdma_replace_sa_options {
	/** Keep the segment open */
	FDMA_REPLACE_SA_OPEN_BIT =	0x0000,
	/** Re-present the segment in workspace */
	FDMA_REPLACE_SA_REPRESENT_BIT =	0x0100,
	/** Close the replaced segment to free the workspace memory
	 * associated with the segment. */
	FDMA_REPLACE_SA_CLOSE_BIT =	0x0200
};

/* @} end of enum fdma_replace_sa_options */

/**************************************************************************//**
 @enum fdma_pta_size_type

 @Description	PTA size type.

 @{
*//***************************************************************************/
enum fdma_pta_size_type {
	/** The frame has no PTA	*/
	PTA_SIZE_NO_PTA = 0x0,
	/** Only the first 32B of the PTA are valid and were presented. */
	PTA_SIZE_PTV1 = 0x1,
	/** Only the second 32B of the PTA are valid and were presented. */
	PTA_SIZE_PTV2 = 0x2,
	/** 64B of the PTA are valid and were presented. */
	PTA_SIZE_PTV1_2 = 0x3
};

/* @} end of enum fdma_pta_size_type */

/* @} end of group FDMA_Enumerations */


/**************************************************************************//**
 @Group		FDMA_Commands_Errors FDMA Commands Errors

 @Description	FDMA Commands Errors

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group		FDMA_PRESENT_FRAME_ERRORS

@Description	FDMA Errors Status returned from Present Frame command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_PRESENT_FRAME_SUCCESS FDMA_SUCCESS
/** Unable to fulfill specified segment presentation size. */
#define FDMA_PRESENT_FRAME_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR		\
		FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR
/** ASAPO value beyond ASAL in received FD. No ASA presentation possible. */
#define FDMA_PRESENT_FRAME_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR		\
		FDMA_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR
/** Unable to fulfill specified ASAPS on an initial frame
 * presentation. Only ASAL amount of annotation was presented.*/
#define FDMA_PRESENT_FRAME_UNABLE_TO_PRESENT_FULL_ASA_ERR		\
		FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR
/** Frame Handle depletion (max of 6). */
#define FDMA_PRESENT_FRAME_FRAME_HANDLE_DEPLETION_ERR			\
		FDMA_FRAME_HANDLE_DEPLETION_ERR
/** Segment Handle depletion (max of 8). */
#define FDMA_PRESENT_FRAME_SEGMENT_HANDLE_DEPLETION_ERR			\
		FDMA_SEGMENT_HANDLE_DEPLETION_ERR
/** Invalid DMA command arguments. */
#define FDMA_PRESENT_FRAME_INVALID_DMA_COMMAND_ARGS_ERR			\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Received non-zero FD[ERR] field from Work Scheduler.
 *  This Error can occur only on autonomous frame presentation (not on
 *  commanded frame presentation). */
#define FDMA_PRESENT_FRAME_FD_ERR					\
		FDMA_FD_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_PRESENT_FRAME_INTERNAL_MEMORY_ECC_ERR			\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_PRESENT_FRAME_WORKSPACE_MEMORY_READ_ERR			\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_PRESENT_FRAME_WORKSPACE_MEMORY_WRITE_ERR			\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** System memory read error (permission or ECC). */
#define FDMA_PRESENT_FRAME_SYSTEM_MEMORY_READ_ERR			\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** Frame structural error (invalid S/G bits settings, hop limit). */
#define FDMA_PRESENT_FRAME_STRUCTURAL_ERR				\
		FDMA_FRAME_STRUCTURAL_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_PRESENT_FRAME_INTERNAL_ERR					\
		FDMA_INTERNAL_ERR

/* @} end of group FDMA_PRESENT_FRAME_ERRORS */


/**************************************************************************//**
@Group		FDMA_PRESENT_SEGMENT_ERRORS

@Description	FDMA Errors Status returned from Present Data Segment command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_PRESENT_SEGMENT_SUCCESS					\
		FDMA_SUCCESS
/** Unable to fulfill specified segment presentation size. */
#define FDMA_PRESENT_SEGMENT_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR		\
		FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR
/** ASAPO value beyond ASAL in received FD. No ASA presentation possible. */
#define FDMA_PRESENT_SEGMENT_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR		\
		FDMA_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR
/** Unable to fulfill specified ASAPS on an initial frame
 * presentation. Only ASAL amount of annotation was presented.*/
#define FDMA_PRESENT_SEGMENT_UNABLE_TO_PRESENT_FULL_ASA_ERR		\
		FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR
/** Frame Handle depletion (max of 6). */
#define FDMA_PRESENT_SEGMENT_INVALID_FRAME_HANDLE_ERR			\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Segment Handle depletion (max of 8). */
#define FDMA_PRESENT_SEGMENT_SEGMENT_HANDLE_DEPLETION_ERR		\
		FDMA_SEGMENT_HANDLE_DEPLETION_ERR
/** Invalid DMA command arguments. */
#define FDMA_PRESENT_SEGMENT_INVALID_DMA_COMMAND_ARGS_ERR		\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_PRESENT_SEGMENT_INTERNAL_MEMORY_ECC_ERR			\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_PRESENT_SEGMENT_WORKSPACE_MEMORY_READ_ERR			\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_PRESENT_SEGMENT_WORKSPACE_MEMORY_WRITE_ERR			\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** System memory read error (permission or ECC). */
#define FDMA_PRESENT_SEGMENT_SYSTEM_MEMORY_READ_ERR			\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** Frame structural error (invalid S/G bits settings, hop limit). */
#define FDMA_PRESENT_SEGMENT_FRAME_STRUCTURAL_ERR			\
		FDMA_FRAME_STRUCTURAL_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_PRESENT_SEGMENT_INTERNAL_ERR				\
		FDMA_INTERNAL_ERR
/** The segment handle does not represent a Data segment. */
#define FDMA_PRESENT_SEGMENT_NO_DATA_SEGMENT_HANDLE			\
		FDMA_NO_DATA_SEGMENT_HANDLE

/* @} end of group FDMA_PRESENT_SEGMENT_ERRORS */


/**************************************************************************//**
@Group		FDMA_PRESENT_ASA_SEGMENT_ERRORS

@Description	FDMA Errors Status returned from Present ASA Segment command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_PRESENT_ASA_SEGMENT_SUCCESS				\
		FDMA_SUCCESS
/** Unable to fulfill specified segment presentation size. */
#define FDMA_PRESENT_ASA_SEGMENT_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR	\
		FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR
/** ASAPO value beyond ASAL in received FD. No ASA presentation possible. */
#define FDMA_PRESENT_ASA_SEGMENT_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR	\
		FDMA_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR
/** Unable to fulfill specified ASAPS on an initial frame
 * presentation. Only ASAL amount of annotation was presented.*/
#define FDMA_PRESENT_ASA_SEGMENT_UNABLE_TO_PRESENT_FULL_ASA_ERR		\
		FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR
/** Frame Handle depletion (max of 6). */
#define FDMA_PRESENT_ASA_SEGMENT_INVALID_FRAME_HANDLE_ERR		\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Segment Handle depletion (max of 8). */
#define FDMA_PRESENT_ASA_SEGMENT_SEGMENT_HANDLE_DEPLETION_ERR		\
		FDMA_SEGMENT_HANDLE_DEPLETION_ERR
/** Invalid DMA command arguments. */
#define FDMA_PRESENT_ASA_SEGMENT_INVALID_DMA_COMMAND_ARGS_ERR		\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_PRESENT_ASA_SEGMENT_INTERNAL_MEMORY_ECC_ERR		\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_PRESENT_ASA_SEGMENT_WORKSPACE_MEMORY_READ_ERR		\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_PRESENT_ASA_SEGMENT_WORKSPACE_MEMORY_WRITE_ERR		\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** System memory read error (permission or ECC). */
#define FDMA_PRESENT_ASA_SEGMENT_SYSTEM_MEMORY_READ_ERR			\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** Frame structural error (invalid S/G bits settings, hop limit). */
#define FDMA_PRESENT_ASA_SEGMENT_FRAME_STRUCTURAL_ERR			\
		FDMA_FRAME_STRUCTURAL_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_PRESENT_ASA_SEGMENT_INTERNAL_ERR				\
		FDMA_INTERNAL_ERR

/* @} end of group FDMA_PRESENT_ASA_SEGMENT_ERRORS */

/**************************************************************************//**
@Group		FDMA_PRESENT_PTA_SEGMENT_ERRORS

@Description	FDMA Errors Status returned from Present PTA Segment command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_PRESENT_PTA_SEGMENT_SUCCESS				\
		FDMA_SUCCESS
/** Unable to fulfill specified segment presentation size. */
#define FDMA_PRESENT_PTA_SEGMENT_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR	\
		FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR
/** ASAPO value beyond ASAL in received FD. No ASA presentation possible. */
#define FDMA_PRESENT_PTA_SEGMENT_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR	\
		FDMA_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR
/** Unable to fulfill specified ASAPS on an initial frame
 * presentation. Only ASAL amount of annotation was presented.*/
#define FDMA_PRESENT_PTA_SEGMENT_UNABLE_TO_PRESENT_FULL_ASA_ERR		\
		FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR
/** Frame Handle depletion (max of 6). */
#define FDMA_PRESENT_PTA_SEGMENT_INVALID_FRAME_HANDLE_ERR		\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Segment Handle depletion (max of 8). */
#define FDMA_PRESENT_PTA_SEGMENT_SEGMENT_HANDLE_DEPLETION_ERR		\
		FDMA_SEGMENT_HANDLE_DEPLETION_ERR
/** Invalid DMA command arguments. */
#define FDMA_PRESENT_PTA_SEGMENT_INVALID_DMA_COMMAND_ARGS_ERR		\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_PRESENT_PTA_SEGMENT_INTERNAL_MEMORY_ECC_ERR		\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_PRESENT_PTA_SEGMENT_WORKSPACE_MEMORY_READ_ERR		\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_PRESENT_PTA_SEGMENT_WORKSPACE_MEMORY_WRITE_ERR		\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** System memory read error (permission or ECC). */
#define FDMA_PRESENT_PTA_SEGMENT_SYSTEM_MEMORY_READ_ERR			\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** Frame structural error (invalid S/G bits settings, hop limit). */
#define FDMA_PRESENT_PTA_SEGMENT_FRAME_STRUCTURAL_ERR			\
		FDMA_FRAME_STRUCTURAL_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_PRESENT_PTA_SEGMENT_INTERNAL_ERR				\
		FDMA_INTERNAL_ERR
/** Invalid PTA address (\ref PRC_PTA_NOT_LOADED_ADDRESS). */
#define FDMA_PRESENT_PTA_SEGMENT_INVALID_PTA_ADDRESS			\
		FDMA_INVALID_PTA_ADDRESS

/* @} end of group FDMA_PRESENT_PTA_SEGMENT_ERRORS */

/**************************************************************************//**
@Group		FDMA_EXTEND_SEGMENT_ERRORS

@Description	FDMA Errors Status returned from Extend Segment command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_EXTEND_SEGMENT_SUCCESS					\
		FDMA_SUCCESS
/** Unable to fulfill specified segment presentation size. */
#define FDMA_EXTEND_SEGMENT_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR		\
		FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR
/** Unable to fulfill specified ASAPS on an initial frame
 * presentation. Only ASAL amount of annotation was presented.*/
#define FDMA_EXTEND_SEGMENT_UNABLE_TO_PRESENT_FULL_ASA_ERR		\
		FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR
/** Invalid Frame Handle. */
#define FDMA_EXTEND_SEGMENT_INVALID_FRAME_HANDLE_ERR			\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Invalid Segment Handle. */
#define FDMA_EXTEND_SEGMENT_INVALID_SEGMENT_HANDLE_ERR			\
		FDMA_INVALID_SEGMENT_HANDLE_ERR
/** Invalid DMA command arguments. */
#define FDMA_EXTEND_SEGMENT_INVALID_DMA_COMMAND_ARGS_ERR		\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_EXTEND_SEGMENT_INTERNAL_MEMORY_ECC_ERR			\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_EXTEND_SEGMENT_WORKSPACE_MEMORY_READ_ERR			\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_EXTEND_SEGMENT_WORKSPACE_MEMORY_WRITE_ERR			\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** System memory read error (permission or ECC). */
#define FDMA_EXTEND_SEGMENT_SYSTEM_MEMORY_READ_ERR			\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** Frame structural error (invalid S/G bits settings, hop limit). */
#define FDMA_EXTEND_SEGMENT_FRAME_STRUCTURAL_ERR			\
		FDMA_FRAME_STRUCTURAL_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_EXTEND_SEGMENT_INTERNAL_ERR				\
		FDMA_INTERNAL_ERR

/* @} end of group FDMA_EXTEND_SEGMENT_ERRORS */


/**************************************************************************//**
@Group		FDMA_STORE_FRAME_ERRORS

@Description	FDMA Errors Status returned from Store Frame command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_STORE_FRAME_SUCCESS					\
		FDMA_SUCCESS
/** Failed due to buffer pool depletion. */
#define FDMA_STORE_FRAME_BUFFER_POOL_DEPLETION_ERR			\
		FDMA_BUFFER_POOL_DEPLETION_ERR
/** Frame Store failed, single buffer frame full and Storage
 * Profile FF is set to 10. */
#define FDMA_STORE_FRAME_FRAME_STORE_ERR				\
		FDMA_FRAME_STORE_ERR
/** Invalid Frame Handle. */
#define FDMA_STORE_FRAME_INVALID_FRAME_HANDLE_ERR			\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_STORE_FRAME_INTERNAL_MEMORY_ECC_ERR			\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_STORE_FRAME_WORKSPACE_MEMORY_READ_ERR			\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_STORE_FRAME_WORKSPACE_MEMORY_WRITE_ERR			\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** System memory read error (permission or ECC). */
#define FDMA_STORE_FRAME_SYSTEM_MEMORY_READ_ERR				\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** System memory write error (permission or ECC). */
#define FDMA_STORE_FRAME_SYSTEM_MEMORY_WRITE_ERR			\
		FDMA_SYSTEM_MEMORY_WRITE_ERR
/** QMan enqueue error (access violation). */
#define FDMA_STORE_FRAME_QMAN_ENQUEUE_ERR				\
		FDMA_QMAN_ENQUEUE_ERR
/** Frame structural error (invalid S/G bits settings, hop
 * limit). */
#define FDMA_STORE_FRAME_FRAME_STRUCTURAL_ERR				\
		FDMA_FRAME_STRUCTURAL_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_STORE_FRAME_INTERNAL_ERR FDMA_INTERNAL_ERR
/** Storage Profile ICID does not match frame ICID and Storage
 * Profile BS=1 error. */
#define FDMA_STORE_FRAME_SPID_ICID_ERR					\
		FDMA_SPID_ICID_ERR

/* @} end of group FDMA_STORE_FRAME_ERRORS */


/**************************************************************************//**
@Group		FDMA_ENQUEUE_FRAME_ERRORS

@Description	FDMA Errors Status returned from Enqueue working frame command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_ENQUEUE_FRAME_SUCCESS					\
	FDMA_SUCCESS
/** Enqueue failed due to QMan enqueue rejection. */
#define FDMA_ENQUEUE_FRAME_ENQUEUE_FAILED_ERR				\
		FDMA_ENQUEUE_FAILED_ERR
/** Failed due to buffer pool depletion. */
#define FDMA_ENQUEUE_FRAME_BUFFER_POOL_DEPLETION_ERR			\
		FDMA_BUFFER_POOL_DEPLETION_ERR
/** Frame Store failed, single buffer frame full and Storage
 * Profile FF is set to 10. */
#define FDMA_ENQUEUE_FRAME_FRAME_STORE_ERR				\
		FDMA_FRAME_STORE_ERR
/** Invalid Frame Handle. */
#define FDMA_ENQUEUE_FRAME_INVALID_FRAME_HANDLE_ERR			\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Invalid DMA command arguments. */
#define FDMA_ENQUEUE_FRAME_INVALID_DMA_COMMAND_ARGS_ERR			\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Invalid DMA command. */
#define FDMA_ENQUEUE_FRAME_INVALID_DMA_COMMAND_ERR			\
		FDMA_INVALID_DMA_COMMAND_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_ENQUEUE_FRAME_INTERNAL_MEMORY_ECC_ERR			\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_ENQUEUE_FRAME_WORKSPACE_MEMORY_READ_ERR			\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_ENQUEUE_FRAME_WORKSPACE_MEMORY_WRITE_ERR			\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** System memory read error (permission or ECC). */
#define FDMA_ENQUEUE_FRAME_SYSTEM_MEMORY_READ_ERR			\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** System memory write error (permission or ECC). */
#define FDMA_ENQUEUE_FRAME_SYSTEM_MEMORY_WRITE_ERR			\
		FDMA_SYSTEM_MEMORY_WRITE_ERR
/** QMan enqueue error (access violation). */
#define FDMA_ENQUEUE_FRAME_QMAN_ENQUEUE_ERR				\
		FDMA_QMAN_ENQUEUE_ERR
/** Frame structural error (invalid S/G bits settings, hop
 * limit). */
#define FDMA_ENQUEUE_FRAME_FRAME_STRUCTURAL_ERR				\
		FDMA_FRAME_STRUCTURAL_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_ENQUEUE_FRAME_INTERNAL_ERR					\
		FDMA_INTERNAL_ERR
/** Storage Profile ICID does not match frame ICID and Storage
 * Profile BS=1 error. */
#define FDMA_ENQUEUE_FRAME_SPID_ICID_ERR				\
		FDMA_SPID_ICID_ERR

/* @} end of group FDMA_ENQUEUE_FRAME_ERRORS */

/**************************************************************************//**
@Group		FDMA_ENQUEUE_FD_ERRORS

@Description	FDMA Errors Status returned from Enqueue FD command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_ENQUEUE_FD_SUCCESS						\
	FDMA_SUCCESS
/** Enqueue failed due to QMan enqueue rejection. */
#define FDMA_ENQUEUE_FD_ENQUEUE_FAILED_ERR				\
		FDMA_ENQUEUE_FAILED_ERR
/** Invalid DMA command arguments. */
#define FDMA_ENQUEUE_FD_INVALID_DMA_COMMAND_ARGS_ERR			\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Workspace memory read Error. */
#define FDMA_ENQUEUE_FD_WORKSPACE_MEMORY_READ_ERR			\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** QMan enqueue error (access violation). */
#define FDMA_ENQUEUE_FD_QMAN_ENQUEUE_ERR				\
		FDMA_QMAN_ENQUEUE_ERR

/* @} end of group FDMA_ENQUEUE_FD_ERRORS */

/**************************************************************************//**
@Group		FDMA_DISCARD_FRAME_ERRORS

@Description	FDMA Errors Status returned from Discard Working Frame command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_DISCARD_FRAME_SUCCESS					\
		FDMA_SUCCESS
/** Invalid Frame Handle. */
#define FDMA_DISCARD_FRAME_INVALID_FRAME_HANDLE_ERR			\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_DISCARD_FRAME_INTERNAL_MEMORY_ECC_ERR			\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_DISCARD_FRAME_WORKSPACE_MEMORY_READ_ERR			\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** System memory read error (permission or ECC). */
#define FDMA_DISCARD_FRAME_SYSTEM_MEMORY_READ_ERR				\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** Frame structural error (invalid S/G bits settings, hop
 * limit). */
#define FDMA_DISCARD_FRAME_FRAME_STRUCTURAL_ERR				\
		FDMA_FRAME_STRUCTURAL_ERR

/* @} end of group FDMA_DISCARD_FRAME_ERRORS */

/**************************************************************************//**
@Group		FDMA_REPLICATE_FRAME_ERRORS

@Description	FDMA Errors Status returned from Replicate Working Frame command

@{
*//***************************************************************************/

/** Success. */
#define FDMA_REPLICATE_FRAME_SUCCESS					\
	FDMA_SUCCESS
/** Enqueue failed due to QMan enqueue rejection. */
#define FDMA_REPLICATE_FRAME_ENQUEUE_FAILED_ERR				\
		FDMA_ENQUEUE_FAILED_ERR
/** Failed due to buffer pool depletion. */
#define FDMA_REPLICATE_FRAME_BUFFER_POOL_DEPLETION_ERR			\
		FDMA_BUFFER_POOL_DEPLETION_ERR
/** Frame Store failed, single buffer frame full and Storage
 * Profile FF is set to 10. */
#define FDMA_REPLICATE_FRAME_FRAME_STORE_ERR				\
		FDMA_FRAME_STORE_ERR
/** Frame Handle depletion (max of 6). */
#define FDMA_REPLICATE_FRAME_FRAME_HANDLE_DEPLETION_ERR			\
		FDMA_FRAME_HANDLE_DEPLETION_ERR
/** Invalid Frame Handle. */
#define FDMA_REPLICATE_FRAME_INVALID_FRAME_HANDLE_ERR			\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_REPLICATE_FRAME_INTERNAL_MEMORY_ECC_ERR			\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_REPLICATE_FRAME_WORKSPACE_MEMORY_READ_ERR			\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_REPLICATE_FRAME_WORKSPACE_MEMORY_WRITE_ERR			\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** System memory read error (permission or ECC). */
#define FDMA_REPLICATE_FRAME_SYSTEM_MEMORY_READ_ERR			\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** System memory write error (permission or ECC). */
#define FDMA_REPLICATE_FRAME_SYSTEM_MEMORY_WRITE_ERR			\
		FDMA_SYSTEM_MEMORY_WRITE_ERR
/** QMan enqueue error (access violation). */
#define FDMA_REPLICATE_FRAME_QMAN_ENQUEUE_ERR				\
		FDMA_QMAN_ENQUEUE_ERR
/** Frame structural error (invalid S/G bits settings, hop limit). */
#define FDMA_REPLICATE_FRAME_FRAME_STRUCTURAL_ERR			\
		FDMA_FRAME_STRUCTURAL_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_REPLICATE_FRAME_INTERNAL_ERR				\
		FDMA_INTERNAL_ERR
/** Storage Profile ICID does not match frame ICID and Storage
 * Profile BS=1 error. */
#define FDMA_REPLICATE_FRAME_SPID_ICID_ERR				\
		FDMA_SPID_ICID_ERR

/* @} end of group FDMA_REPLICATE_FRAME_ERRORS */


/**************************************************************************//**
@Group		FDMA_CONCATENATE_FRAMES_ERRORS

@Description	FDMA Errors Status returned from Concatenate Working Frames
		command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_CONCATENATE_FRAMES_SUCCESS					\
		FDMA_SUCCESS
/** Failed due to buffer pool depletion. */
#define FDMA_CONCATENATE_FRAMES_BUFFER_POOL_DEPLETION_ERR		\
		FDMA_BUFFER_POOL_DEPLETION_ERR
/** Unable to trim frame to concatenate, trim size is larger
 * than frame size. */
#define FDMA_CONCATENATE_FRAMES_UNABLE_TO_TRIM_ERR			\
		FDMA_UNABLE_TO_TRIM_ERR
/** Invalid Frame Handle. */
#define FDMA_CONCATENATE_FRAMES_INVALID_FRAME_HANDLE_ERR		\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_CONCATENATE_FRAMES_INTERNAL_MEMORY_ECC_ERR			\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_CONCATENATE_FRAMES_WORKSPACE_MEMORY_READ_ERR		\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_CONCATENATE_FRAMES_WORKSPACE_MEMORY_WRITE_ERR		\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** System memory read error (permission or ECC). */
#define FDMA_CONCATENATE_FRAMES_SYSTEM_MEMORY_READ_ERR			\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** System memory write error (permission or ECC). */
#define FDMA_CONCATENATE_FRAMES_SYSTEM_MEMORY_WRITE_ERR			\
		FDMA_SYSTEM_MEMORY_WRITE_ERR
/** Frame structural error (invalid S/G bits settings, hop limit). */
#define FDMA_CONCATENATE_FRAMES_FRAME_STRUCTURAL_ERR			\
		FDMA_FRAME_STRUCTURAL_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_CONCATENATE_FRAMES_INTERNAL_ERR				\
		FDMA_INTERNAL_ERR
/** Storage Profile ICID does not match frame ICID and Storage
 * Profile BS=1 error. */
#define FDMA_CONCATENATE_FRAMES_SPID_ICID_ERR				\
		FDMA_SPID_ICID_ERR

/* @} end of group FDMA_CONCATENATE_FRAMES_ERRORS */

/**************************************************************************//**
@Group		FDMA_SPLIT_FRAME_ERRORS

@Description	FDMA Errors Status returned from Split Working Frame command

@{
*//***************************************************************************/

/** Success. */
#define FDMA_SPLIT_FRAME_SUCCESS					\
		FDMA_SUCCESS
/** Failed due to buffer pool depletion. */
#define FDMA_SPLIT_FRAME_BUFFER_POOL_DEPLETION_ERR			\
		FDMA_BUFFER_POOL_DEPLETION_ERR
/** Unable to split frame, split size is larger than frame
 * size or no SF bit found. */
#define FDMA_SPLIT_FRAME_UNABLE_TO_SPLIT_ERR				\
		FDMA_UNABLE_TO_SPLIT_ERR
/** Frame Store failed, single buffer frame full and Storage
 * Profile FF is set to 10. */
#define FDMA_SPLIT_FRAME_FRAME_STORE_ERR				\
		FDMA_FRAME_STORE_ERR
/** Unable to fulfill specified segment presentation size. */
#define FDMA_SPLIT_FRAME_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR		\
		FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR
/** Frame Handle depletion (max of 6). */
#define FDMA_SPLIT_FRAME_FRAME_HANDLE_DEPLETION_ERR			\
		FDMA_FRAME_HANDLE_DEPLETION_ERR
/** Invalid Frame Handle. */
#define FDMA_SPLIT_FRAME_INVALID_FRAME_HANDLE_ERR			\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Segment Handle depletion (max of 8). */
#define FDMA_SPLIT_FRAME_SEGMENT_HANDLE_DEPLETION_ERR			\
		FDMA_SEGMENT_HANDLE_DEPLETION_ERR
/** Invalid DMA command arguments. */
#define FDMA_SPLIT_FRAME_INVALID_DMA_COMMAND_ARGS_ERR			\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_SPLIT_FRAME_INTERNAL_MEMORY_ECC_ERR			\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_SPLIT_FRAME_WORKSPACE_MEMORY_READ_ERR			\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_SPLIT_FRAME_WORKSPACE_MEMORY_WRITE_ERR			\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** System memory read error (permission or ECC). */
#define FDMA_SPLIT_FRAME_SYSTEM_MEMORY_READ_ERR				\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** System memory write error (permission or ECC). */
#define FDMA_SPLIT_FRAME_SYSTEM_MEMORY_WRITE_ERR			\
		FDMA_SYSTEM_MEMORY_WRITE_ERR
/** Frame structural error (invalid S/G bits settings, hop limit). */
#define FDMA_SPLIT_FRAME_FRAME_STRUCTURAL_ERR				\
		FDMA_FRAME_STRUCTURAL_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_SPLIT_FRAME_INTERNAL_ERR					\
		FDMA_INTERNAL_ERR
/** Storage Profile ICID does not match frame ICID and Storage
 * Profile BS=1 error. */
#define FDMA_SPLIT_FRAME_SPID_ICID_ERR					\
		FDMA_SPID_ICID_ERR

/* @} end of group FDMA_SPLIT_FRAME_ERRORS */

/**************************************************************************//**
@Group		FDMA_TRIM_SEGMENT_ERRORS

@Description	FDMA Errors Status returned from Trim Segment Presentation
		command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_TRIM_SEGMENT_SUCCESS					\
		FDMA_SUCCESS
/** Invalid Frame Handle. */
#define FDMA_TRIM_SEGMENT_INVALID_FRAME_HANDLE_ERR			\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Invalid Segment Handle. */
#define FDMA_TRIM_SEGMENT_INVALID_SEGMENT_HANDLE_ERR			\
		FDMA_INVALID_SEGMENT_HANDLE_ERR
/** Invalid DMA command arguments. */
#define FDMA_TRIM_SEGMENT_INVALID_DMA_COMMAND_ARGS_ERR			\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_TRIM_SEGMENT_INTERNAL_MEMORY_ECC_ERR			\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_TRIM_SEGMENT_WORKSPACE_MEMORY_READ_ERR			\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_TRIM_SEGMENT_INTERNAL_ERR					\
		FDMA_INTERNAL_ERR

/* @} end of group FDMA_TRIM_SEGMENT_ERRORS */

/**************************************************************************//**
@Group		FDMA_REPLACE_DATA_SEGMENT_ERRORS

@Description	FDMA Errors Status returned from Replace Data Segment
		command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_REPLACE_DATA_SEGMENT_SUCCESS				\
		FDMA_SUCCESS
/** Unable to fulfill specified segment presentation size. */
#define FDMA_REPLACE_DATA_SEGMENT_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR	\
		FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR
/** Unable to fulfill specified ASAPS on an initial frame
 * presentation. Only ASAL amount of annotation was presented.*/
#define FDMA_REPLACE_DATA_SEGMENT_UNABLE_TO_PRESENT_FULL_ASA_ERR	\
		FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR
/** Invalid Frame Handle. */
#define FDMA_REPLACE_DATA_SEGMENT_INVALID_FRAME_HANDLE_ERR		\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Invalid Segment Handle. */
#define FDMA_REPLACE_DATA_SEGMENT_INVALID_SEGMENT_HANDLE_ERR		\
		FDMA_INVALID_SEGMENT_HANDLE_ERR
/** Invalid DMA command arguments. */
#define FDMA_REPLACE_DATA_SEGMENT_INVALID_DMA_COMMAND_ARGS_ERR		\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_REPLACE_DATA_SEGMENT_INTERNAL_MEMORY_ECC_ERR		\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_REPLACE_DATA_SEGMENT_WORKSPACE_MEMORY_READ_ERR		\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_REPLACE_DATA_SEGMENT_WORKSPACE_MEMORY_WRITE_ERR		\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_REPLACE_DATA_SEGMENT_INTERNAL_ERR				\
		FDMA_INTERNAL_ERR
/** The segment handle does not represent a Data segment. */
#define FDMA_REPLACE_DATA_SEGMENT_NO_DATA_SEGMENT_HANDLE		\
		FDMA_NO_DATA_SEGMENT_HANDLE

/* @} end of group FDMA_REPLACE_DATA_SEGMENT_ERRORS */

/**************************************************************************//**
@Group		FDMA_REPLACE_ASA_SEGMENT_ERRORS

@Description	FDMA Errors Status returned from Replace ASA Segment
		command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_REPLACE_ASA_SEGMENT_SUCCESS				\
		FDMA_SUCCESS
/** Unable to fulfill specified segment presentation size. */
#define FDMA_REPLACE_ASA_SEGMENT_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR	\
		FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR
/** Unable to fulfill specified ASAPS on an initial frame
 * presentation. Only ASAL amount of annotation was presented.*/
#define FDMA_REPLACE_ASA_SEGMENT_UNABLE_TO_PRESENT_FULL_ASA_ERR	\
		FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR
/** Invalid Frame Handle. */
#define FDMA_REPLACE_ASA_SEGMENT_INVALID_FRAME_HANDLE_ERR		\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Invalid Segment Handle. */
#define FDMA_REPLACE_ASA_SEGMENT_INVALID_SEGMENT_HANDLE_ERR		\
		FDMA_INVALID_SEGMENT_HANDLE_ERR
/** Invalid DMA command arguments. */
#define FDMA_REPLACE_ASA_SEGMENT_INVALID_DMA_COMMAND_ARGS_ERR		\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_REPLACE_ASA_SEGMENT_INTERNAL_MEMORY_ECC_ERR		\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_REPLACE_ASA_SEGMENT_WORKSPACE_MEMORY_READ_ERR		\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_REPLACE_ASA_SEGMENT_WORKSPACE_MEMORY_WRITE_ERR		\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_REPLACE_ASA_SEGMENT_INTERNAL_ERR				\
		FDMA_INTERNAL_ERR

/* @} end of group FDMA_REPLACE_ASA_SEGMENT_ERRORS */


/**************************************************************************//**
@Group		FDMA_REPLACE_PTA_SEGMENT_ERRORS

@Description	FDMA Errors Status returned from Replace PTA Segment
		command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_REPLACE_PTA_SEGMENT_SUCCESS				\
		FDMA_SUCCESS
/** Unable to fulfill specified segment presentation size. */
#define FDMA_REPLACE_PTA_SEGMENT_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR	\
		FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR
/** Unable to fulfill specified ASAPS on an initial frame
 * presentation. Only ASAL amount of annotation was presented.*/
#define FDMA_REPLACE_PTA_SEGMENT_UNABLE_TO_PRESENT_FULL_ASA_ERR	\
		FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR
/** Invalid Frame Handle. */
#define FDMA_REPLACE_PTA_SEGMENT_INVALID_FRAME_HANDLE_ERR		\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Invalid Segment Handle. */
#define FDMA_REPLACE_PTA_SEGMENT_INVALID_SEGMENT_HANDLE_ERR		\
		FDMA_INVALID_SEGMENT_HANDLE_ERR
/** Invalid DMA command arguments. */
#define FDMA_REPLACE_PTA_SEGMENT_INVALID_DMA_COMMAND_ARGS_ERR		\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_REPLACE_PTA_SEGMENT_INTERNAL_MEMORY_ECC_ERR		\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_REPLACE_PTA_SEGMENT_WORKSPACE_MEMORY_READ_ERR		\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** Workspace memory write Error. */
#define FDMA_REPLACE_PTA_SEGMENT_WORKSPACE_MEMORY_WRITE_ERR		\
		FDMA_WORKSPACE_MEMORY_WRITE_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_REPLACE_PTA_SEGMENT_INTERNAL_ERR				\
		FDMA_INTERNAL_ERR
/** Invalid PTA address (\ref PRC_PTA_NOT_LOADED_ADDRESS). */
#define FDMA_REPLACE_PTA_SEGMENT_INVALID_PTA_ADDRESS			\
		FDMA_INVALID_PTA_ADDRESS

/* @} end of group FDMA_REPLACE_PTA_SEGMENT_ERRORS */

/**************************************************************************//**
@Group		FDMA_CHECKSUM_ERRORS

@Description	FDMA Errors Status returned from Calculate Checksum
		command

@{
*//***************************************************************************/
/** Success. */
#define FDMA_CHECKSUM_SUCCESS						\
		FDMA_SUCCESS
/** Invalid Frame Handle. */
#define FDMA_CHECKSUM_INVALID_FRAME_HANDLE_ERR				\
		FDMA_INVALID_FRAME_HANDLE_ERR
/** Invalid DMA command arguments. */
#define FDMA_CHECKSUM_INVALID_DMA_COMMAND_ARGS_ERR			\
		FDMA_INVALID_DMA_COMMAND_ARGS_ERR
/** Internal memory ECC uncorrected ECC error. */
#define FDMA_CHECKSUM_INTERNAL_MEMORY_ECC_ERR				\
		FDMA_INTERNAL_MEMORY_ECC_ERR
/** Workspace memory read Error. */
#define FDMA_CHECKSUM_WORKSPACE_MEMORY_READ_ERR				\
		FDMA_WORKSPACE_MEMORY_READ_ERR
/** System memory read error (permission or ECC). */
#define FDMA_CHECKSUM_SYSTEM_MEMORY_READ_ERR				\
		FDMA_SYSTEM_MEMORY_READ_ERR
/** Frame structural error (invalid S/G bits settings, hop limit). */
#define FDMA_CHECKSUM_FRAME_STRUCTURAL_ERR				\
		FDMA_FRAME_STRUCTURAL_ERR
/** FDMA Internal error, SRU depletion. */
#define FDMA_CHECKSUM_INTERNAL_ERR					\
		FDMA_INTERNAL_ERR

/* @} end of group FDMA_CHECKSUM_ERRORS */



/* @} end of group FDMA_Commands_Errors */

/**************************************************************************//**
 @Group		FDMA_Commands_Flags FDMA Commands Flags

 @Description	FDMA Commands Flags

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group		FDMA_Present_Frame_Flags

@Description	FDMA Frame Presentation flags

@{
*//***************************************************************************/
	/** No flags indication. */
#define FDMA_INIT_NO_FLAGS	0x00000000
	/** No Data Segment.
	 * If set - do not present Data segment. Otherwise - present Data. */
#define FDMA_INIT_NDS_BIT	0x00000200
	/** Reference within the frame to present from.
	 * If set - end of the frame. Otherwise - start of the frame. */
#define FDMA_INIT_SR_BIT	0x00000100

/* @} end of group FDMA_Present_Frame_Flags */

/**************************************************************************//**
@Group		FDMA_PRES_Flags

@Description	FDMA Present segment flags

@{
*//***************************************************************************/

	/** No flags indication. */
#define FDMA_PRES_NO_FLAGS	0x00000000
	/** Reference within the frame to present from (This field is ignored
	 * when presenting PTA segments. For ASA segments it indicates segment
	 * reference within the annotation data.).
	 * If set - end of the frame. Otherwise - start of the frame. */
#define FDMA_PRES_SR_BIT	0x100

/* @} end of group FDMA_PRES_Flags */

/**************************************************************************//**
@Group		FDMA_EXT_Flags

@Description	FDMA Extend segment flags

@{
*//***************************************************************************/

	/** No flags indication. */
#define FDMA_EXT_NO_FLAGS	0x00000000
	/** The type of segment to present. Only one option may be choose from
	 * \ref fdma_st_options. */
#define FDMA_EXT_ST_BIT	fdma_st_options

/* @} end of group FDMA_EXT_Flags */

/**************************************************************************//**
@Group		FDMA_ENWF_Flags

@Description	FDMA Enqueue working frame flags

@{
*//***************************************************************************/

	/** No flags indication. */
#define FDMA_ENWF_NO_FLAGS	0x00000000
	/** Terminate Control options. Only one option may be choose from
	 * \ref fdma_enqueue_tc_options. */
#define FDMA_ENWF_TC		fdma_enqueue_tc_options
	/** Enqueue Priority source.
	 * Relevant for Queuing Destination Selection.
	 * If set - use QD_PRI from h/w context (this is the value found in the
	 * WQID field from ADC). Otherwise - use QD_PRI provided with DMA
	 * Command. */
#define FDMA_ENWF_PS_BIT	0x00001000

/* @} end of group FDMA_ENWF_Flags */

/**************************************************************************//**
@Group		FDMA_ENF_Flags

@Description	FDMA Enqueue frame flags

@{
*//***************************************************************************/

	/** No flags indication. */
#define FDMA_ENF_NO_FLAGS	0x00000000
	/** Terminate Control options. Only one option may be choose from
	 * \ref fdma_enqueue_tc_options. */
#define FDMA_ENF_TC		fdma_enqueue_tc_options
	/** Enqueue Priority source.
	 * Relevant for Queuing Destination Selection.
	 * If set - use QD_PRI from h/w context (this is the value found in the
	 * WQID field from ADC). Otherwise - use QD_PRI provided with DMA
	 * Command. */
#define FDMA_ENF_PS_BIT		0x00001000
	/** Virtual Address. */
#define FDMA_ENF_VA_BIT		0x00002000
	/** Bypass the Memory Translation. */
#define FDMA_ENF_BMT_BIT	0x00004000
	/** Privilege Level. */
#define FDMA_ENF_PL_BIT		0x00008000
	/** Bypass DPAA resource Isolation.
	 * If set - Isolation is not enabled for this command (the FQID ID
	 * specified is a real (not virtual) pool ID). Otherwise - Isolation is
	 * enabled for this command (the FQID ID specified is virtual within the
	 * specified ICID). */
#define FDMA_ENF_BDI_BIT	0x80000000

/* @} end of group FDMA_ENF_Flags */


/**************************************************************************//**
@Group		FDMA_Discard_WF_Flags

@Description	FDMA Discard working frame flags

@{
*//***************************************************************************/

	/** No flags indication. */
#define FDMA_DIS_NO_FLAGS	0x00000000
	/** Terminate Control.
	 * If set - Trigger the Terminate task command right after the discard.
	 * Otherwise - Return after discard. */
#define FDMA_DIS_WF_TC_BIT	0x00000100
/* Todo -
	* Frame Source: Discard working frame (using frame handle).
#define FDMA_DIS_FS_HANDLE_BIT	0x0000
	* Frame Source: Discard Frame (using frame FD).
#define FDMA_DIS_FS_FD_BIT	0x0200
*/


/* @} end of group FDMA_Discard_WF_Flags */


/**************************************************************************//**
@Group		FDMA_Replicate_Flags

@Description	FDMA Replicate Working Frame flags

@{
*//***************************************************************************/

	/** No flags indication. */
#define FDMA_REPLIC_NO_FLAGS	0x00000000
	/** Enqueue the replicated frame to the provided Queueing Destination.
	 * Release destination frame handle is implicit when enqueueing.
	 * If set - replicate and enqueue. Otherwise - replicate only. */
#define FDMA_REPLIC_ENQ_BIT	0x00000400
	/** The source frame resources are released after the replication.
	 * Release source frame handle is implicit when discarding.
	 * If set - discard source frame and release frame handle.
	 * Otherwise - keep source frame. */
#define FDMA_REPLIC_DSF_BIT	0x00000800
	/** Enqueue Priority source.
	 * Relevant for Queuing Destination Selection.
	 * If set - use QD_PRI from h/w context.
	 * Otherwise - use QD_PRI provided with DMA Command. */
#define FDMA_REPLIC_PS_BIT	0x00001000
	/** AIOP FDMA copy frame annotations options. Only one option may be
	 * choose from \ref fdma_cfa_options. */
#define FDMA_REPLICATE_CFA	fdma_cfa_options

/* @} end of group FDMA_Replicate_Flags */


/**************************************************************************//**
@Group		FDMA_Concatenate_Flags

@Description	FDMA Concatenate Frames flags

@{
*//***************************************************************************/

	/** No flags indication. */
#define FDMA_CONCAT_NO_FLAGS	0x00000000
	/** Set SF bit on SGE of the original frames. Note, this will force the
	 * usage of SG format on the concatenated frame.
	 * If set - Set SF bit. Otherwise - Do not set SF bit. */
#define FDMA_CONCAT_SF_BIT	0x00000100
	/** Post Concatenate Action.
	 * If set - close resulting working frame 1 using provided storage
	 * profile, update FD1.
	 * Otherwise - keep resulting working frame 1 open. */
#define FDMA_CONCAT_PCA_BIT	0x00000800

/* @} end of group FDMA_Concatenate_Flags */

/**************************************************************************//**
@Group		FDMA_Split_Flags

@Description	FDMA Split Frame flags

@{
*//***************************************************************************/

	/** No flags indication. */
#define FDMA_SPLIT_NO_FLAGS	0x00000000
	/** AIOP FDMA copy frame annotations options. Only one option may be
	 * choose from \ref fdma_cfa_options. */
#define FDMA_SPLIT_CFA		fdma_cfa_options
	/** AIOP FDMA Post split action options. Only one option may be
	 * choose from \ref fdma_split_psa_options. */
#define FDMA_SPLIT_PSA		fdma_split_psa_options
	/** Frame split mode.
	 * If set - Split is performed based on Scatter Gather Entry SF bit.
	 * Otherwise - Split is performed at the split_size_sf parameter value
	 * of the split command. */
#define FDMA_SPLIT_SM_BIT	0x00000200
	/** Reference within the frame to present from.
	 * If set - end of the frame. Otherwise - start of the frame. */
#define FDMA_SPLIT_SR_BIT	0x00000100

/* @} end of group FDMA_Split_Flags */

/**************************************************************************//**
@Group		FDMA_Replace_Flags

@Description	FDMA Replace working frame segment flags

@{
*//***************************************************************************/

	/** No flags indication. */
#define FDMA_REPLACE_NO_FLAGS	0x00000000
	/** Segment action options. Only one option may be
	 * choose from \ref fdma_replace_sa_options. */
#define FDMA_REPLACE_SA		fdma_replace_sa_options

/* @} end of group FDMA_Replace_Flags */

/**************************************************************************//**
@Group		FDMA_ICID_CONTEXT_Flags

@Description	ICID context flags

@{
*//***************************************************************************/

	/** Virtual Address of the Stored frame flag. */
#define FDMA_ICID_CONTEXT_VA	0x00002000
	/** Bypass the Memory Translation of the Stored frame flag. */
#define FDMA_ICID_CONTEXT_BMT	0x00004000
	/** Privilege Level of the Stored frame flag. */
#define FDMA_ICID_CONTEXT_PL	0x00008000

/* @} end of group FDMA_ICID_CONTEXT_Flags */



/* @} end of group FDMA_Commands_Flags */


/**************************************************************************//**
 @Group		FDMA_Structures FDMA Structures

 @Description	FDMA Structures

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	Working Frame structure.

		Includes a presented frame information.

*//***************************************************************************/
struct working_frame {
		/** A pointer to Frame descriptor in workspace */
	struct ldpaa_fd *fd;
		/** Handle to the HW working frame */
	uint8_t frame_handle;
		/** 32-bit alignment. */
	uint8_t	pad[3];
};


/**************************************************************************//**
@Description	Segment structure.

		Includes a presented segment information.

*//***************************************************************************/
struct segment {
		/** A pointer to the Working Frame holding the segment */
	struct working_frame *wf;
		/** A pointer to the address within the workspace of the
			 * presented segment.*/
	void *ws_address;
		/** The number of segments bytes presented */
	uint16_t seg_length;
		/** Handle to the presented segment. */
	uint8_t	 seg_handle;

		/** Segment flags \ref FDMA_PRES_Flags */
	uint8_t	 flags;
		/** Segment offset in the frame relative to the
		 * \ref FDMA_PRES_SR_BIT flag */
	uint16_t seg_offset;
		/** 32-bit alignment. */
	uint8_t	pad[2];
};


/**************************************************************************//**
@Description	Initial frame presentation parameters structure.

*//***************************************************************************/
struct fdma_present_frame_params {
		/** \link FDMA_Present_Frame_Flags initial frame presentation
		 * flags \endlink */
	uint32_t flags;
		/** A pointer to the location in workspace for the presented
		 * frame segment. */
	void *seg_dst;
		/** A pointer to a 64B-aligned location in the workspace to
		 * store the 64B PTA field (set \ref PRC_PTA_NOT_LOADED_ADDRESS
		 * for no PTA presentation). */
	void *pta_dst;
		/** A pointer to a 64B-aligned location in the workspace to
		 * store the ASA. */
	void *asa_dst;
		/** A pointer to the location in workspace of the FD that is to
		* be presented. */
	void *fd_src;
		/** location within the presented frame to start presenting
		 * the segment from. */
	uint16_t seg_offset;
		/** Number of frame bytes to present and create an open
		 * segment for. */
	uint16_t present_size;
		/** The first ASA 64B quantity to present. */
	uint8_t asa_offset;
		/** Number (maximum) of 64B ASA quantities to present. */
	uint8_t asa_size;
		/** Returned parameter:
		* The number of bytes actually presented (the segment actual
		* size). */
	uint16_t seg_length;
		/** Returned parameter:
		 * The handle of the working frame. */
	uint8_t frame_handle;
		/** Returned parameter:
		 * The handle of the presented segment. */
	uint8_t seg_handle;
		/** 32-bit alignment. */
	uint8_t	pad[2];
};

/**************************************************************************//**
@Description	Queueing Destination Enqueue parameters structure.

*//***************************************************************************/
struct fdma_queueing_destination_params {
		/**< Queueing destination for the enqueue. */
	uint16_t qd;
		/** Distribution hash value passed to QMan for distribution
		 * purpose on the enqueue. */
	uint16_t hash_value;
		/** Queueing Destination Priority. */
	uint8_t	 qd_priority;
		/** 32-bit alignment. */
	uint8_t	pad[3];
};

/**************************************************************************//**
@Description	Concatenate frames parameters structure.

*//***************************************************************************/
struct fdma_concatenate_frames_params {
		/** \link FDMA_Concatenate_Flags concatenate frames
		 * flags \endlink */
	uint32_t  flags;
		/** The handle of working frame 1. */
	uint16_t frame1;
		/** The handle of working frame 2. */
	uint16_t frame2;
		/** Storage Profile used to store frame data if additional
		 * buffers are required when optionally closing the concatenated
		 *  working frame (\ref FDMA_CONCAT_PCA_BIT is set) */
	uint8_t  spid;
		/** Trim a number of bytes from the beginning of frame 2 before
		 * the concatenation. A size of zero disables the trim. */
	uint8_t  trim;
		/** 32-bit alignment. */
	uint8_t	pad[2];
};

/**************************************************************************//**
@Description	Split frame parameters structure.

*//***************************************************************************/
struct fdma_split_frame_params {
		/** \link FDMA_Split_Flags split frames flags
		 * \endlink */
	uint32_t flags;
		/** A pointer to the location in workspace for the split FD. */
	void *fd_dst;
		/** A pointer to the location in workspace for the presented
		 * split frame segment. */
	void *seg_dst;
		/** location within the presented split frame to start
		 * presenting the segment from. */
	uint16_t seg_offset;
		/** Number of frame bytes to present and create an open segment
		 * for (in the split frame). */
	uint16_t present_size;
		/** SM bit = 0: Split size, number of bytes to split from the
		 * head of the input frame and move to the output frame.
		 * SM bit = FDMA_SPLIT_SM_BIT: Backward offset from the SGE
		 * marked with the SF bit, used by the FDMA to modify the start
		 * offset in the structure to recover frame data that is
		 * currently in the structure but not part of the frame.
		 * A value of 0 will have no effect on start offset. */
	uint16_t split_size_sf;
		/** Returned parameter:
		 * The number of bytes actually presented from the split frame
		 * (the segment actual size). */
	uint16_t seg_length;
		/** The handle of the source working frame. */
	uint8_t  source_frame_handle;
		/**< Returned parameter:
		 * A pointer to the handle of the split working frame. */
	uint8_t  split_frame_handle;
		/** Returned parameter:
		 * A pointer to the handle of the presented segment (in the
		 * split frame). */
	uint8_t  seg_handle;
		/** Storage Profile used to store frame data if additional
		 * buffers are required when optionally closing the split
		 * working frame */
	uint8_t  spid;
};

/**************************************************************************//**
@Description	ICID Context parameters structure.

*//***************************************************************************/
struct fdma_icid_context_params {
		/** \link FDMA_ICID_CONTEXT_Flags icid context flags. \endlink*/
	uint32_t flags;
		/**
		 * bits<0> : BDI of the Stored frame.
		 * bits<1-15> : ICID of the Stored frame. */
	uint16_t bdi_icid;
		/** 32-bit alignment. */
	uint8_t	pad[2];
};

/* @} end of group FDMA_Structures */


/**************************************************************************//**
 @Group		FDMA_Functions FDMA Functions

 @Description	FDMA Functions

 @{
*//***************************************************************************/


/* FPDMA (Frame Presentation DMA) Commands from AIOP Software */

/**************************************************************************//**
@Function	fdma_present_default_frame

@Description	Initial presentation of a default working frame into the task
		workspace.

		Implicit input parameters in Task Defaults: segment address,
		segment offset, segment size, PTA address (\ref
		PRC_PTA_NOT_LOADED_ADDRESS for no presentation), ASA address,
		ASA size, ASA offset, Segment Reference bit, fd address in
		workspace (\ref HWC_FD_ADDRESS).

		This command can also be used to initiate construction of a
		frame from scratch (without a presented frame). In this case
		the fd address parameter must point to a null FD (all 0x0) in
		the workspace, and an empty segment must be allocated (of size
		0).

		Implicitly updated values in Task Defaults:  frame handle,
		segment handle.

@Return		Status - Success or Failure (e.g. DMA error. (\ref
		FDMA_PRESENT_FRAME_ERRORS)).

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_present_default_frame(void);

/**************************************************************************//**
@Function	fdma_present_frame

@Description	Initial presentation of the frame into the task
		workspace.

		This command can also be used to initiate construction of a
		frame from scratch (without a presented frame). In this case
		the fd address parameter must point to a null FD (all 0x0) in
		the workspace, and an empty segment must be allocated (of size
		0).

		In case the fd destination parameter points to the default FD
		address, the service routine will update Task defaults variables
		according to command parameters.

@Param[in,out]	params - A pointer to the Initial frame presentation command
		parameters.

@Return		Status - Success or Failure (e.g. DMA error. (\ref
		FDMA_PRESENT_FRAME_ERRORS)).

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_present_frame(
		struct fdma_present_frame_params *params);

/**************************************************************************//**
@Function	fdma_present_default_frame_segment

@Description	Open a segment of the default working frame and copy the
		segment data into the specified location in the workspace.

		Implicit input parameters in Task Defaults: frame handle.

@Param[in]	flags - \link FDMA_PRES_Flags Present segment flags. \endlink
@Param[in]	ws_dst - A pointer to the location in workspace for the
		presented frame segment.
@Param[in]	offset - Location within the presented frame to start presenting
		from. Must be within the bound of the frame.
@Param[in]	present_size - Number of frame bytes to present (Must be greater
		than 0).
@Param[out]	seg_length - A pointer to the number of bytes actually
		presented (the segment actual size).
@Param[out]	seg_handle - A pointer to the handle of the presented segment.

@Return		Status - Success or Failure (e.g. DMA error. (\ref
		FDMA_PRESENT_SEGMENT_ERRORS).

@Cautions	This command may be invoked only for Data segments.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_present_default_frame_segment(
		uint32_t flags,
		void	 *ws_dst,
		uint16_t offset,
		uint16_t present_size,
		uint16_t *seg_length,
		uint8_t  *seg_handle);

/**************************************************************************//**
@Function	fdma_read_default_frame_asa

@Description	Read the Accelerator Specific Annotation (ASA) section
		associated with the default working frame from external memory
		into a specified location in the workspace.

		Implicit input parameters in Task Defaults: frame handle.

		Implicitly updated values in Task Defaults: ASA segment address,
		ASA segment length (the number of bytes actually presented given
		in 64B units), ASA segment offset.

@Param[in]	flags - \link FDMA_PRES_Flags Present segment flags. \endlink
@Param[in]	offset - Location within the ASA to start presenting from.
		Must be within the bound of the frame. Specified in 64B units.
@Param[in]	present_size - Number of frame bytes to present (Must be greater
		than 0). Contains the number of 64B quantities to present
		because the Frame ASAL field is specified in 64B units.
@Param[in]	ws_dst - A pointer to the location in workspace for the
		presented ASA segment.

@Return		Success or Failure (e.g. DMA error, No HW Annotation. (\ref
		FDMA_PRESENT_ASA_SEGMENT_ERRORS)).

@remark		The ASA segment handle value is fixed \ref FDMA_ASA_SEG_HANDLE.

@Cautions	The HW must have previously opened the frame with an
		automatic initial presentation or initial presentation command.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_read_default_frame_asa(
		uint32_t flags,
		uint16_t offset,
		uint16_t present_size,
		void	 *ws_dst);

/**************************************************************************//**
@Function	fdma_read_default_frame_pta

@Description	Read the Pass Through Annotation (PTA) section associated with
		the default working frame from external memory into a specified
		location in the workspace.

		Implicit input parameters in Task Defaults: frame handle.

		Implicitly updated values in Task Defaults: PTA segment address.

@Param[in]	ws_dst - A pointer to the location in workspace for the
		presented PTA segment.

@Return		Success or Failure (e.g. DMA error, No SW Annotation. (\ref
		FDMA_PRESENT_PTA_SEGMENT_ERRORS)).

@remark		The PTA segment handle value is fixed \ref FDMA_PTA_SEG_HANDLE.

@remark		The length of the read PTA can be read directly from the FD.

@Cautions	The HW must have previously opened the frame with an
		automatic initial presentation or initial presentation command.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_read_default_frame_pta(void *ws_dst);

/**************************************************************************//**
@Function	fdma_extend_default_segment_presentation

@Description	Extend an existing presentation default segment with additional
		data.

		This command can present frame data, and frame accelerator
		specific annotation data (ASA).

		Implicitly updated values in Task Defaults: segment (Data or
		ASA) length. For ASA segment, specifies the number of additional
		64B quantities to present from the ASA.

@Param[in]	extend_size - Number of additional bytes to present (0 results
		in no operation.)
@Param[in]	ws_dst - A pointer to the location within the workspace
		to present the additional frame segment data.
@Param[in]	flags - \link FDMA_EXT_Flags extend segment mode
		bits. \endlink

@Return		Status - Success or Failure (e.g. DMA error. (\ref
		FDMA_EXTEND_SEGMENT_ERRORS)).

@remark		The extended data to be presented does not have to be
		sequential relative to the current presented segment.
@remark		If this command is executed on the default data segment then
		after this command the default segment values in the
		presentation context will not be valid.

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_extend_default_segment_presentation(
		uint16_t extend_size,
		void	 *ws_dst,
		uint32_t flags);


/* FDMA (Frame Output DMA) Commands from AIOP Software */

/**************************************************************************//**
@Function	fdma_store_default_frame_data

@Description	Write out modified default Working Frame to the backing storage
		in system memory described by the Frame Descriptor.

		Existing FD buffers are used to store data.

		If the modified frame no longer fits in the original structure,
		new buffers can be added using the provided storage profile.
		If the original structure can not be modified, then a new
		structure will be assembled using the default frame storage
		profile ID.

		Implicit input parameters in Task Defaults: frame handle,
		spid (storage profile ID).

@Return
		- Success or Failure (e.g. FDMA error. (\ref
		FDMA_STORE_FRAME_ERRORS)).
		- Update FD.

@Cautions	All modified segments (which are to be stored) must be replaced
		(by a replace command) before storing a frame.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_store_default_frame_data(void);

/**************************************************************************//**
@Function	fdma_store_frame_data

@Description	Write out modified Working Frame to the backing storage
		in system memory described by the Frame Descriptor.

		Existing FD buffers are used to store data.

		If the modified frame no longer fits in the original structure,
		new buffers can be added using the provided storage profile.
		If the original structure can not be modified, then a new
		structure will be assembled using the frame storage
		profile ID parameter.

@Param[in]	frame_handle - Handle to the frame to be closed.
@Param[in]	spid - storage profile ID used to store frame data if additional
		buffers are required.
@Param[out]	icid_context - Icid context structure.

@Return
		- Success or Failure (e.g. FDMA error. (\ref
		FDMA_STORE_FRAME_ERRORS)).
		- Update FD.

@Cautions	All modified segments (which are to be stored) must be replaced
		(by a replace command) before storing a frame.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_store_frame_data(
		uint8_t frame_handle,
		uint8_t spid,
		struct fdma_icid_context_params *icid_context);

/**************************************************************************
@Function	fdma_close_frame

@Description	Closes a Working Frame. Data which was written to the working
		frame will be written to the backing storage in system memory
		described by the Frame Descriptor.

		Existing FD buffers are used to store data.

		If the modified frame no longer fits in the original structure,
		new buffers can be added using the provided storage profile.
		If the original structure can not be modified, then a new
		structure will be assembled using the default frame storage
		profile ID.

		This service routine is intended to be used with frames which
		are not the default frame.

		Implicit input parameters in Task Defaults: spid (storage
		profile ID).

@Param[in]	frame_handle - Handle to the frame to be closed.

@Return
		- Success or Failure (e.g. FDMA error. (\ref fdma_hw_errors)).
		- Update FD.

@Cautions	All modified segments (which are to be stored) must be replaced
		(by a replace command) before storing a frame.
@Cautions	In this Service Routine the task yields.
*************************************************************************
int32_t fdma_close_frame(uint16_t frame_handle);*/

/**************************************************************************//**
@Function	fdma_store_and_enqueue_default_frame_fqid

@Description	Enqueue the default Working Frame to a given destination
		according to a frame queue id.

		After completion, the Enqueue Working Frame command can
		terminate the task or return.

		If the Working Frame to be enqueued is modified, the Enqueue
		Frame command performs a Store Frame Data command on the
		Working Frame.

		If the Working Frame to be enqueued is modified, existing
		buffers as described by the FD are used to store data.

		If the modified frame no longer fits in the original structure,
		new buffers can be added using the provided storage profile.

		If the original structure can not be modified, then a new
		structure will be assembled using the default frame storage
		profile ID.

		Implicit input parameters in Task Defaults: frame handle, spid
		(storage profile ID).

@Param[in]	fqid - frame queue ID for the enqueue.
@Param[in]	flags - \link FDMA_ENWF_Flags enqueue working frame mode
		bits. \endlink

@Return		Status (if enqueue succeeded or failed.
		(\ref FDMA_ENQUEUE_FRAME_ERRORS)).

@Cautions
		- Function may not return.
		- All modified segments (which are to be stored) must be
		replaced (by a replace command) before storing a frame.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_store_and_enqueue_default_frame_fqid(
		uint32_t fqid,
		uint32_t flags);

/**************************************************************************//**
@Function	fdma_store_and_enqueue_default_frame_qd

@Description	Enqueue the default Working Frame to a given destination
		according to a queueing destination.

		After completion, the Enqueue Working Frame command can
		terminate the task or return.

		If the Working Frame to be enqueued is modified, the Enqueue
		Frame command performs a Store Frame Data command on the
		Working Frame.

		If the Working Frame to be enqueued is modified, existing
		buffers as described by the FD are used to store data.

		If the modified frame no longer fits in the original structure,
		new buffers can be added using the provided storage profile.

		If the original structure can not be modified, then a new
		structure will be assembled using the default frame storage
		profile ID.

		Implicit input parameters in Task Defaults: frame handle, spid
		(storage profile ID).

@Param[in]	enqueue_params - Pointer to the queueing destination parameters.
@Param[in]	flags - \link FDMA_ENWF_Flags enqueue working frame mode
		bits. \endlink

@Return		Status (if enqueue succeeded or failed. (\ref
		FDMA_ENQUEUE_FRAME_ERRORS)).

@Cautions
		- Function may not return.
		- All modified segments (which are to be stored) must be
		replaced (by a replace command) before storing a frame.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_store_and_enqueue_default_frame_qd(
		struct fdma_queueing_destination_params *enqueue_params,
		uint32_t	flags);

/**************************************************************************//**
@Function	fdma_enqueue_default_fd_fqid

@Description	Enqueue the default FD (which is not presented) to a given
		destination according to a frame queue id.

		After completion, the Enqueue Frame command can
		terminate the task or return.

		Implicit input parameters in Task Defaults: fd address.

@Param[in]	icid - ICID of the FD to enqueue.
@Param[in]	flags - \link FDMA_ENF_Flags enqueue frame flags.
		\endlink
@Param[in]	fqid - frame queue ID for the enqueue.


@Return		Status (if enqueue succeeded or failed. (\ref
		FDMA_ENQUEUE_FD_ERRORS)).

@Cautions	Function may not return.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_enqueue_default_fd_fqid(
		uint16_t icid ,
		uint32_t flags,
		uint32_t fqid);

/**************************************************************************//**
@Function	fdma_enqueue_default_fd_qd

@Description	Enqueue the default FD (which is not presented) to a given
		destination according to a queueing destination.

		After completion, the Enqueue Frame command can
		terminate the task or return.

		Implicit input parameters in Task Defaults: fd address.

@Param[in]	icid - ICID of the FD to enqueue.
@Param[in]	flags - \link FDMA_ENF_Flags enqueue frame flags.
		\endlink
@Param[in]	enqueue_params - Pointer to the queueing destination parameters.


@Return		Status (if enqueue succeeded or failed. (\ref
		FDMA_ENQUEUE_FD_ERRORS)).

@Cautions	Function may not return.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_enqueue_default_fd_qd(
		uint16_t icid ,
		uint32_t flags,
		struct fdma_queueing_destination_params *enqueue_params);

/**************************************************************************//**
@Function	fdma_discard_default_frame

@Description	Release the resources associated with the default working frame.

		Implicit input parameters in Task Defaults: frame handle.

@Param[in]	flags - \link FDMA_Discard_WF_Flags discard frame flags. \endlink

@Return		Status (Success or Failure. (\ref FDMA_DISCARD_FRAME_ERRORS)).

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_discard_default_frame(uint32_t flags);
/**************************************************************************//**
@Function	fdma_discard_frame

@Description	Release the resources associated with a working frame.

@Param[in]	frame - Frame handle to be discarded.
@Param[in]	flags - \link FDMA_Discard_WF_Flags discard working frame /
		frame flags. \endlink

@Return		Status (Success or Failure. (\ref FDMA_DISCARD_FRAME_ERRORS)).

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_discard_frame(uint16_t frame, uint32_t flags);

/**************************************************************************//**
@Function	fdma_terminate_task

@Description	End all processing on the associated task and notify the Order
		Scope Manager of the task termination.

@Return		None.

@Cautions
		- Application software must store (in software managed context)
		or discard the input frame before calling Terminate task
		command to avoid buffer leak.
		- Function does not return
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
void fdma_terminate_task(void);

/**************************************************************************//**
@Function	fdma_replicate_frame_fqid

@Description	Make a copy of the working frame and optionally enqueue the
		replicated frame (the copy) according to a frame queue id.

		The source Working Frame may be modified but all the segments
		must be closed.

@Param[in]	frame_handle1 - Handle of the source frame.
@Param[in]	spid - Storage Profile used to store frame data of the
		destination frame if \ref FDMA_REPLIC_ENQ_BIT is selected, also
		used to determine ICID and memory attributes of the replicated
		frame.
@Param[in]	fqid - frame queue ID for the enqueue.
@Param[in]	fd_dst - A pointer to the location within the workspace of the
		destination FD.
@Param[in]	flags - \link FDMA_Replicate_Flags replicate working frame
		flags \endlink.
@Param[out]	frame_handle2 - Handle of the replicated frame (when no enqueue
		selected).

Handle of the replicated frame (when no enqueue selected)

@Return		Status (Success or Failure. (\ref FDMA_REPLICATE_FRAME_ERRORS))

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_replicate_frame_fqid(
		uint8_t	frame_handle1,
		uint8_t	spid,
		uint32_t fqid,
		void *fd_dst,
		uint32_t flags,
		uint8_t *frame_handle2
		);

/**************************************************************************//**
@Function	fdma_replicate_frame_qd

@Description	Make a copy of the working frame and optionally enqueue the
		replicated frame (the copy) according to a queueing destination.

		The source Working Frame may be modified but all the segments
		must be closed.

@Param[in]	frame_handle1 - Handle of the source frame.
@Param[in]	spid - Storage Profile used to store frame data of the
		destination frame if \ref FDMA_REPLIC_ENQ_BIT is selected, also
		used to determine ICID and memory attributes of the replicated
		frame.
@Param[in]	enqueue_params - Pointer to the queueing destination parameters.
@Param[in]	fd_dst - A pointer to the location within the workspace of the
		destination FD.
@Param[in]	flags - \link FDMA_Replicate_Flags replicate working frame
		flags \endlink.
@Param[out]	frame_handle2 - Handle of the replicated frame (when no enqueue
		was selected).

Handle of the replicated frame (when no enqueue selected)

@Return		Status (Success or Failure. (\ref FDMA_REPLICATE_FRAME_ERRORS))

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_replicate_frame_qd(
		uint8_t	frame_handle1,
		uint8_t	spid,
		struct fdma_queueing_destination_params *enqueue_params,
		void *fd_dst,
		uint32_t flags,
		uint8_t *frame_handle2
		);

/**************************************************************************//**
@Function	fdma_concatenate_frames

@Description	Join two frames {frame1 , frame2} and return a new concatenated
		frame.

		Pre-condition: The two frames may be modified but all
		the segments must be closed.

		The command also support the option to trim a number of
		bytes from the beginning of the 2nd frame before it is
		concatenated.

		The frames must be in the same ICID.

@Param[in,out]	params - A pointer to the Concatenate frames command parameters.

@Return		Status (Success or Failure. (\ref
		FDMA_CONCATENATE_FRAMES_ERRORS)).

@remark		Frame annotation of the first frame becomes the frame annotation
		of the concatenated frame

@Cautions
		- In case frame1 handle parameter is the default frame handle,
		the default frame length variable in the Task defaults will not
		be valid after the service routine.
		- In case frame2 handle parameter is the default frame handle,
		all Task default variables will not be valid after the service
		routine.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_concatenate_frames(
		struct fdma_concatenate_frames_params *params);

/**************************************************************************//**
@Function	fdma_split_frame

@Description	Split a Working Frame into two frames and return an updated
		Working Frame along with a split frame.

		The source Working Frame may be modified but all the segments
		must be Closed.

		In case the fd destination parameter points to the default FD
		address, the service routine will update Task defaults
		variables according to command parameters.

@Param[in,out]	params - A pointer to the Split frame command parameters

@Return		Status (Success or Failure. (\ref FDMA_SPLIT_FRAME_ERRORS)).

@remark
		- The first fd is updated to reflect the remainder of the
		input fd (the second part of the split frame).
		- The second fd represent the split portion of the frame (the
		first part of the split frame).
		- Frame annotation of the first frame is preserved.

@remark		If split size is >= frame size then an error will be returned.

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_split_frame(
		struct fdma_split_frame_params *params);

/**************************************************************************//**
@Function	fdma_trim_default_segment_presentation

@Description	Re-size down a previously presented default Data segment
		(head/tail trim).

		Updates the presented data identified with the segment (Does
		not do a replace command). The change will only take place in
		the FDMA side. After this command the FDMA will associate the
		segment with a subset of the original segment of 'size' bytes
		starting from a specified offset in the original segment (the
		new size and offset are the Service Routine parameters).

		Implicit input parameters in Task Defaults: frame handle,
		segment handle.

		Implicitly updated values in task defaults: segment length.

@Param[in]	offset - Offset from the previously presented segment
		representing the new start of the segment (head trim).
@Param[in]	size - New segment size in bytes.

@Return		Status (Success or Failure. (\ref FDMA_TRIM_SEGMENT_ERRORS)).

@remark		Example: Trim segment to 20 bytes at offset 10.
		The default Data segment represents a 100 bytes at offset 0 in
		the frame (0-99) and the user want to the segment to represent
		only 20 bytes starting at offset 10 (relative to the presented
		segment).
		Parameters:
			- offset - 10 (relative to the presented segment)
			- size - 20

@Cautions	This command may be invoked only for Data segments.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_trim_default_segment_presentation(
		uint16_t offset, uint16_t size);


/**************************************************************************//**
@Function	fdma_modify_default_segment_data

@Description	Modified data in the default Data segment in the default
		Working Frame (in the FDMA).

		This Service Routine updates the FDMA that certain data in the
		presented segment was modified. The updated data is located in
		the same place the old data was located at in the segment
		presentation in workspace.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	offset - The offset from the previously presented segment
		representing the start point of the modification.
		Must be within the presented segment size.
@Param[in]	size - The Working Frame modified size.

@Return		Status (Success or Failure. (\ref
		FDMA_REPLACE_DATA_SEGMENT_ERRORS)).

@remark		Example: Modify 14 bytes. The default Data segment represents a
		100 bytes at offset 0 in the frame (0-99) and the user has
		updated bytes 11-24 (14 bytes) at their original location in the
		segment presentation in workspace.
		Parameters:
			- offset - 11 (relative to the presented segment)
			- size - 14

@Cautions	This command may be invoked only on the default Data segment.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_modify_default_segment_data(
		uint16_t offset,
		uint16_t size);

/**************************************************************************//**
@Function	fdma_replace_default_segment_data

@Description	Replace modified data in the default Data segment in the default
		Working Frame (in the FDMA).

		This Service Routine can replace any data size in the segment
		with any new data size. the new data will be taken from
		from_ws_src pointer parameter.

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the segment data between the
		Task Workspace and the FDMA.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle.

		Implicitly updated values in task defaults: segment length,
		segment address.

@Param[in]	to_offset - The offset from the previously presented segment
		representing the start point of the replacement.
		Must be within the presented segment size.
@Param[in]	to_size - The Working Frame replaced size.
@Param[in]	from_ws_src - a pointer to the workspace location from which
		the replacement segment data starts.
@Param[in]	from_size - The replacing segment size.
@Param[in]	ws_dst_rs - A pointer to the location in workspace for the
		represented frame segment (relevant if
		\ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set).
@Param[in]	size_rs - Number of frame bytes to represent (relevant if
		\ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set).
@Param[in]	flags - \link FDMA_Replace_Flags replace working frame
		segment flags. \endlink

@Return		Status (Success or Failure. (\ref
		FDMA_REPLACE_DATA_SEGMENT_ERRORS)).

@remark		Example: Modify 14 bytes + insert 2 bytes. The default Data
		segment represents a 100 bytes at offset 0 in the frame (0-99)
		and the user want to replace bytes 11-24 (14 bytes) with new 16
		bytes (14 updated + additional 2).
		Parameters:
			- to_offset - 11 (relative to the presented segment)
			- to_size - 14
			- from_ws_address - <workspace address of the 16 bytes>
			- from_size - 16

@Cautions	This command may be invoked only on the default Data segment.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_replace_default_segment_data(
		uint16_t to_offset,
		uint16_t to_size,
		void	 *from_ws_src,
		uint16_t from_size,
		void	 *ws_dst_rs,
		uint16_t size_rs,
		uint32_t flags);

/**************************************************************************//**
@Function	fdma_insert_default_segment_data

@Description	Insert new data to the default Working Frame (in the FDMA)
		through the default Data segment .

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the segment data between the
		Task Workspace and the FDMA.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle.

		Implicitly updated values in task defaults: segment length,
		segment address.

@Param[in]	to_offset - Offset from the previously presented segment
		representing where to insert the data.
		Must be within the presented segment size.
@Param[in]	from_ws_src - a pointer to the workspace location from which
		the inserted segment data starts.
@Param[in]	insert_size - Size of the data being inserted to the segment.
@Param[in]	flags - \link FDMA_Replace_Flags replace working frame
		segment flags. \endlink

@Return		Status (Success or Failure. (\ref
		FDMA_REPLACE_DATA_SEGMENT_ERRORS)).

@remark
		- This is basically a replace command with
		to_size = 0 (0 bytes are replaced, 'size' bytes are inserted).
		- Example: Insert 2 bytes - The default Data
		segment represents a 100 bytes at offset 0 in the frame (0-99),
		and the user want to insert 2 bytes after the 24th byte in the
		segment.
		Parameters:
			- to_offset - 25 (relative to the presented segment)
			- from_ws_address - <workspace address of the 2 bytes>
			- insert_size - 2

@remark		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set, the
		pointer to the workspace start address of the represented
		segment and the number of frame bytes to represent are
		calculated automatically in the Service Routine according to the
		input parameters and the Task Default variables.
		This is done from performance considerations.
		The workspace start address of the represented segment
		calculated as insert_size bytes before the old segment
		address.
		The number of frame bytes to represent remains the old
		segment length.

@Cautions	In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set, The
		Service Routine assumes there is enough headroom in the
		Workspace before the default segment address to represent the
		segment.
@Cautions	This command may be invoked only on the default Data segment.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_insert_default_segment_data(
		uint16_t to_offset,
		void	 *from_ws_src,
		uint16_t insert_size,
		uint32_t flags);

/**************************************************************************//**
@Function	fdma_delete_default_segment_data

@Description	Delete data from the default Working Frame (in the FDMA) through
		the default Data segment.

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the segment data between the
		Task Workspace and the FDMA.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle.

		Implicitly updated values in task defaults: segment length,
		segment address.

@Param[in]	to_offset - Offset from the previously presented segment
		representing from where to delete data.
		Must be within the presented segment size.
@Param[in]	delete_target_size - Size of the data being deleted from the
		segment.
@Param[in]	flags - \link FDMA_Replace_Flags replace working frame
		segment flags. \endlink

@Return		Status (Success or Failure. (\ref
		FDMA_REPLACE_DATA_SEGMENT_ERRORS)).

@remark
		- This is basically a replace command with
		to_size = delete_target_size, ws_address = irrelevant (0),
		size = 0 (replacing 'delete_target_size' bytes with 0 bytes =
		deletion).
		- Example: Delete 10 bytes. The default Data segment represents
		a 100 bytes at offset 0 in the frame (0-99), and the user want
		to delete 10 bytes after the 24th byte.
		Parameters:
			- to_offset - 25 (relative to the presented segment)
			- delete_target_size - 10

@remark		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set the
		pointer to the workspace start address of the represented
		segment and the number of frame bytes to represent are
		calculated automatically in the Service Routine according to the
		input parameters and the Task Default variables.
		This is done from performance considerations.
		The workspace start address of the represented segment
		calculated as delete_target_size bytes after the old segment
		address.
		The number of frame bytes to represent is the old segment
		length reduced by delete_target_size bytes.

@Cautions	This command may be invoked only on the default Data segment.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_delete_default_segment_data(
		uint16_t to_offset,
		uint16_t delete_target_size,
		uint32_t flags);

/**************************************************************************//**
@Function	fdma_close_default_frame_segment

@Description	Closes the default segment in the default frame.
		Free the workspace memory associated with the segment.
		All segment modifications which were not written to the working
		frame will be lost.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle.

@Return		Status (Success or Failure. (\ref
		FDMA_REPLACE_DATA_SEGMENT_ERRORS)).

@Cautions	This command may be invoked only for Data segments.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_close_default_segment(void);

/**************************************************************************//**
@Function	fdma_replace_default_frame_asa_segment_data

@Description	Replace modified data in the Accelerator Specific Annotation
		(ASA) segment associated with the default Working Frame (in the
		FDMA).

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the ASA segment data between the
		Task Workspace and the FDMA.

		Implicit input parameters in Task Defaults: frame handle,
		ASA segment handle.

		Implicitly updated values in Task Defaults:
		ASA segment address, ASA segment length.

		Implicitly updated values in FD - ASA length.

@Param[in]	to_offset - Offset from the beginning of the ASA data
		representing the start point of the replacement specified in
		64B quantities.
@Param[in]	to_size - The number of 64B quantities that are to be replaced
		within the ASA segment.
@Param[in]	from_ws_src - A pointer to the workspace location from which
		the replacement segment data starts.
@Param[in]	from_size - The number of 64B units that will replace the
		specified portion of the ASA segment.
@Param[in]	ws_dst_rs - A pointer to the location in workspace for the
		represented frame segment (relevant if \ref
		FDMA_REPLACE_SA_REPRESENT_BIT) flag is set).
@Param[in]	size_rs - Number of frame bytes to represent in 64B portions
		(relevant if \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set).
@Param[in]	flags - \link FDMA_Replace_Flags replace working frame
		segment flags. \endlink

@Return		Status (Success or Failure. (\ref
		FDMA_REPLACE_ASA_SEGMENT_ERRORS)).

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_replace_default_asa_segment_data(
		uint16_t to_offset,
		uint16_t to_size,
		void	 *from_ws_src,
		uint16_t from_size,
		void	 *ws_dst_rs,
		uint16_t size_rs,
		uint32_t flags);

/**************************************************************************//**
@Function	fdma_replace_default_pta_segment_data

@Description	Replace modified data in the Pass-Through Annotation (PTA)
		segment associated with the default Working Frame (in the FDMA).

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the PTA segment data between the
		Task Workspace and the FDMA.

		The entire PTA annotation (32 or 64 bytes) is replaced when this
		command is invoked.

		Usage examples:
		- Passing GRO information to GPP.

		Implicit input parameters in Task Defaults: frame handle,
		PTA segment handle, PTA segment address.

		Implicitly updated values in Task Defaults: PTA segment address.

		Implicitly updated values in FD - PTA length.

@Param[in]	flags - \link FDMA_Replace_Flags replace working frame
		segment flags. \endlink
@Param[in]	from_ws_src - a pointer to the workspace location from which
		the replacement segment data starts.
@Param[in]	ws_dst_rs - A pointer to the location in workspace for the
		represented frame segment (relevant if \ref
		FDMA_REPLACE_SA_REPRESENT_BIT flag is set). In case of
		representing the PTA, always represent the full PTA (64 bytes).
@Param[in]	size_type - Replacing segment size type of the PTA
		(\ref fdma_pta_size_type).

@Return		Status (Success or Failure. (\ref
		FDMA_REPLACE_PTA_SEGMENT_ERRORS)).

@remark		The length of the represented PTA can be read directly from the
		FD.

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_replace_default_pta_segment_data(
		uint32_t flags,
		void	 *from_ws_src,
		void	 *ws_dst_rs,
		enum fdma_pta_size_type size_type);

/**************************************************************************//**
@Function	fdma_calculate_checksum

@Description	Computes the gross checksum of the default Working
		Frame.

@Param[in]	offset - Number of bytes offset in the frame from which to start
		calculation of checksum.
@Param[in]	size - Number of bytes to do calculation of checksum.
		Use 0xffff to calculate checksum until the end of the frame.
@Param[out]	checksum - Ones complement sum over the specified range of the
		working frame.

@Return		Success or Failure (\ref FDMA_CHECKSUM_ERRORS)).

@Cautions	The h/w must have previously opened the frame with an
		initial presentation or initial presentation command.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_calculate_default_frame_checksum(
		uint16_t offset,
		uint16_t size,
		uint16_t *checksum);


/** @} */ /* end of FDMA_Functions */

/** @} */ /* end of FSL_AIOP_FDMA */

#endif /* __FSL_FDMA_H_ */
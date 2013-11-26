/**************************************************************************//**
@File		aiop_verification_parser.h

@Description	This file contains the AIOP PARSER SW Verification Structures

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __AIOP_VERIFICATION_PARSER_H_
#define __AIOP_VERIFICATION_PARSER_H_

#include "fsl_ldpaa.h"
#include "fsl_parser.h"
/**************************************************************************//**
 @addtogroup		AIOP_Service_Routines_Verification

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_PARSER_SRs_Verification

 @Description	AIOP PARSER Verification structures definitions.

 @{
*//***************************************************************************/

/** \enum parser_verif_cmd_type defines the parser verification CMDTYPE
 * field. */
enum parser_verif_cmd_type {
	PARSER_PRP_CREATE_VERIF_CMDTYPE = 0,
	PARSER_PRP_DELETE_VERIF_CMDTYPE,
	PARSER_PRP_QUERY_VERIF_CMDTYPE,
	PARSER_GEN_PARSE_RES_VERIF_CMDTYPE,
	PARSER_GEN_PARSE_RES_EXP_VERIF_CMDTYPE
};

#define PARSER_PRP_CREATE_STR  ((CTLU_PARSE_CLASSIFY_ACCEL_ID << 16) | \
				PARSER_PRP_CREATE_VERIF_CMDTYPE)

#define PARSER_PRP_DELETE_STR  ((CTLU_PARSE_CLASSIFY_ACCEL_ID << 16) | \
				PARSER_PRP_DELETE_VERIF_CMDTYPE)

#define PARSER_PRP_QUERY_STR  ((CTLU_PARSE_CLASSIFY_ACCEL_ID << 16) | \
				PARSER_PRP_QUERY_VERIF_CMDTYPE)

#define PARSER_GEN_PARSE_RES_STR  ((CTLU_PARSE_CLASSIFY_ACCEL_ID << 16) | \
				PARSER_GEN_PARSE_RES_VERIF_CMDTYPE)

#define PARSER_GEN_PARSE_RES_EXP_STR  ((CTLU_PARSE_CLASSIFY_ACCEL_ID << 16) | \
					PARSER_GEN_PARSE_RES_EXP_VERIF_CMDTYPE)

/**************************************************************************//**
@Description	Parser Profile Create Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_prp_create_verif_command {
	uint32_t opcode;
	int32_t  status;
	uint32_t parse_profile;
	uint8_t  prpid;
	uint8_t  pad[3];
};

/**************************************************************************//**
@Description	Parser Profile Delete Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_prp_delete_verif_command {
	uint32_t opcode;
	int32_t  status;
	uint8_t  prpid;
	uint8_t  pad[3];
};

/**************************************************************************//**
@Description	Parser Profile Query Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_prp_query_verif_command {
	uint32_t opcode;
	int32_t  status;
	uint32_t parse_profile;
	uint8_t  prpid;
	uint8_t  pad[3];
};

/**************************************************************************//**
@Description	Parser Generate Parser Results Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_gen_parser_res_verif_command {
	uint32_t opcode;
	int32_t  status;
	uint8_t  flags;
	uint8_t  pad[3];
};

/**************************************************************************//**
@Description	Parser Generate Parser Results (EXP) Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_gen_parser_res_exp_verif_command {
	uint32_t                      opcode;
	int32_t                       status;
	enum parser_starting_hxs_code hxs;
	uint8_t                       flags;
	uint8_t                       offset;
	uint8_t                       pad[1];
};

uint16_t aiop_verification_parser(uint32_t asa_seg_addr);

/** @} */ /* end of AIOP_PARSER_SRs_Verification */

/** @} */ /* end of AIOP_Service_Routines_Verification */


#endif /* __AIOP_VERIFICATION_PARSER_H_ */
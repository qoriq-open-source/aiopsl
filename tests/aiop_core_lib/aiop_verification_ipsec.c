/**************************************************************************//**
@File		aiop_verification_ipsec.c

@Description	This file contains the AIOP IPsec SW Verification Commands

		Copyright 2014 Freescale Semiconductor, Inc.
*//***************************************************************************/


#include "aiop_verification.h"
#include "aiop_verification_ipsec.h"
#include "ipsec.h"

uint16_t  aiop_verification_ipsec(uint32_t data_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;

	opcode  = *((uint32_t *) data_addr);

	switch (opcode) {
	case IPSEC_INIT_CMD:
	{
		struct ipsec_init_command *str =
			(struct ipsec_init_command *)data_addr;
		
		str->status = ipsec_init(
				str->max_sa_no);
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_init_command);
		break;
	}
	
	case IPSEC_ADD_SA_DESCRIPTOR_CMD:
	{
		struct ipsec_add_sa_descriptor_command *str =
			(struct ipsec_add_sa_descriptor_command *)data_addr;
		
		str->status = ipsec_add_sa_descriptor(
				&(str->params),
				(uint64_t *)(str->ipsec_handle_ptr)
				);
		
		str->descriptor_addr = *((uint64_t *)(str->ipsec_handle_ptr));
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_add_sa_descriptor_command);
		
		break;
	}
	
	case IPSEC_DEL_SA_DESCRIPTOR_CMD:
	{
		struct ipsec_del_sa_descriptor_command *str =
			(struct ipsec_del_sa_descriptor_command *)data_addr;
		
		str->status = ipsec_del_sa_descriptor(
				*((uint64_t *)(str->ipsec_handle_ptr)));
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_del_sa_descriptor_command);
		
		break;
	}
	
	case IPSEC_GET_LIFETIME_STATS_CMD:
	{
		struct ipsec_get_lifetime_stats_command *str =
			(struct ipsec_get_lifetime_stats_command *)data_addr;
		
		str->status = ipsec_get_lifetime_stats(
				*((uint64_t *)(str->ipsec_handle_ptr)),
				&(str-> kilobytes),
				&(str-> packets),
				&(str-> sec)
				);
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_get_lifetime_stats_command);
		
		break;
	}
	
	case IPSEC_DECR_LIFETIME_COUNTERS_CMD:
	{
		struct ipsec_decr_lifetime_counters_command *str =
			(struct ipsec_decr_lifetime_counters_command *)data_addr;
		
		str->status = ipsec_decr_lifetime_counters(
				*((uint64_t *)(str->ipsec_handle_ptr)),
				str->kilobytes_decr_val,
				str->packets_decr_val
				);
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_decr_lifetime_counters_command);
		
		break;
	}
	
	case IPSEC_GET_SEQ_NUM_CMD:
	{
		struct ipsec_get_seq_num_command *str =
			(struct ipsec_get_seq_num_command *)data_addr;
		
		str->status = ipsec_get_seq_num(
				*((uint64_t *)(str->ipsec_handle_ptr)),
				&(str->sequence_number),
				&(str->extended_sequence_number),
				&(str->anti_replay_bitmap[4])
				);
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_get_seq_num_command);
		
		break;
	}
	
	
	case IPSEC_FRAME_DECRYPT_CMD:
	{
		struct ipsec_frame_decrypt_command *str =
			(struct ipsec_frame_decrypt_command *)data_addr;
		
		str->status = ipsec_frame_decrypt(
				*((uint64_t *)(str->ipsec_handle_ptr)),
				&str->dec_status		
		);
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str->pr = *((struct parse_result *) HWC_PARSE_RES_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_frame_decrypt_command);
		
		break;
	}
	case IPSEC_FRAME_ENCRYPT_CMD:
	{
		struct ipsec_frame_encrypt_command *str =
			(struct ipsec_frame_encrypt_command *)data_addr;
		
		str->status = ipsec_frame_encrypt(
				*((uint64_t *)(str->ipsec_handle_ptr)),
				&str->enc_status		
		);
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str->pr = *((struct parse_result *) HWC_PARSE_RES_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_frame_encrypt_command);
		
		break;
	}
	
	case IPSEC_FRAME_ENCR_DECR_CMD:
	{
		struct ipsec_frame_encr_decr_command *str =
			(struct ipsec_frame_encr_decr_command *)data_addr;
		
		/* Encryption */
		str->fm_encr_status = ipsec_frame_encrypt(
				*((uint64_t *)(str->ipsec_encr_handle_ptr)),
				&str->enc_status		
		);
		*((int32_t *)(str->fm_encr_status_addr)) = str->fm_encr_status;
		
		/* if encryption failed, don't do decryption */
		if (str->fm_encr_status)
			break;
		
		/* Decryption */
		str->fm_decr_status = ipsec_frame_encrypt(
				*((uint64_t *)(str->ipsec_decr_handle_ptr)),
				&str->dec_status		
		);
		*((int32_t *)(str->fm_decr_status_addr)) = str->fm_decr_status;
		
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str->pr = *((struct parse_result *) HWC_PARSE_RES_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_frame_encr_decr_command);
		
		break;
	}
	
	default:
	{
		return STR_SIZE_ERR;
	}
	}
	
	return str_size;
}







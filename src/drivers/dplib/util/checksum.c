/**************************************************************************//**
@File		checksum.c

@Description	This file contains the AIOP SW general checksum functions.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "checksum.h"

uint16_t cksum_ones_complement_sum16(uint16_t arg1, uint16_t arg2)
{
	int32_t tmp = arg1 + arg2;
	return (uint16_t)(tmp + (tmp >> 16));
}

void cksum_update_uint32(register uint16_t *cs_ptr,
		register uint32_t old_val,
		register uint32_t new_val)
{
	register temp1;
	register temp2;
	asm{
		se_lhz	temp1, 0(cs_ptr)	/* Load CS */
		nor	new_val, new_val, new_val/* One's complement of the new
						value. Pipeline optimization */
		addc	temp1, temp1, old_val	/* Adding old value to CS and
						generating carry */
		adde	temp1, new_val, temp1	/* Adding new value and carry of
						the previous addition to CS */
		addze	temp1, temp1		/* Add carry of the previous
						addition to CS */
		e_rlwinm temp2, temp1, 16, 0, 31/* Replace two half words
						location */
		se_add	temp2, temp1		/* Adding two half words and
						first half word carry */
		se_srwi	temp2, 16		/* Isolate only the high 2B of
						the previous addition */
		se_sth	temp2, 0(cs_ptr)	/* Store CS */
	}
}

uint16_t cksum_accumulative_update_uint32(register uint16_t cksum,
		register uint32_t old_val,
		register uint32_t new_val)
{
	register temp2;
	asm{
		nor	new_val, new_val, new_val/* One's complement of the new
						value. Pipeline optimization */
		addc	cksum, cksum, old_val	/* Adding old value to CS and
						generating carry */
		adde	cksum, new_val, cksum	/* Adding new value and carry of
						the previous addition to CS */
		addze	cksum, cksum		/* Add carry of the previous
						addition to CS */
		e_rlwinm  temp2, cksum, 16, 0, 31/* Replace two half words
						location */
		se_add	temp2, cksum		/* Adding two half words and
						first half word carry */
		se_srwi	temp2, 16		/* Isolate only the high 2B of
						the previous addition */
	}
	return (uint16_t) temp2;
}


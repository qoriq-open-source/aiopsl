/**************************************************************************//**
@File		system.c

@Description	This file contains the AIOP SW system interface.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "fsl_system.h"
#include "id_pool.h"

/* Global parameters*/
uint64_t ext_prpid_pool_address;
uint64_t ext_keyid_pool_address;

int32_t sys_ctlu_prpid_pool_create(uint16_t buffer_pool_id,
						uint32_t buffer_size)
{
	int8_t pool[SYS_PRPID_POOL_LENGTH];
	int32_t status;

	status = id_pool_init(pool, SYS_PRPID_POOL_LENGTH, buffer_pool_id,
			 buffer_size, &ext_prpid_pool_address);
	return status;
}


int32_t sys_ctlu_keyid_pool_create(uint16_t buffer_pool_id,
							uint32_t buffer_size)
{
	int8_t pool[SYS_KEYID_POOL_LENGTH];
	int32_t status;

	status = id_pool_init(pool, SYS_PRPID_POOL_LENGTH, buffer_pool_id,
			buffer_size, &ext_keyid_pool_address);
	return status;
}
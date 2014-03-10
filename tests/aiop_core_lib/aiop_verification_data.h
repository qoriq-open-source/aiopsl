/**************************************************************************//**
@File		aiop_verification_data.h

@Description	This file contains the AIOP SW Verification Data
		needed for the test

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_DATA_H_
#define __AIOP_VERIFICATION_DATA_H_

#define __VERIF_GLOBAL __declspec(section ".verif_data")
#define __VERIF_TLS __declspec(section ".verif_tdata")
#define __VERIF_PROFILE_SRAM __declspec(section ".verif_psram_data")

#pragma section RW ".verif_data" ".verif_bss"
#pragma section RW ".verif_tdata" ".verif_tbss"
#pragma section RW ".verif_psram_data" ".verif_psram_bss"

void init_verif();

#endif /* __AIOP_VERIFICATION_DATA_H_ */

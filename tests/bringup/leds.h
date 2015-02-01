/**
@File          leds.h

@Description   header file to turn on leds from AIOP.
               atu_fpga_window_for_leds.patch should be installed on MC
*//***************************************************************************/
#ifndef __LED_H
#define __LED_H

#include "common/types.h"

#define FPGA_VIRT_ADDRESS     0x8010000
#define LED_CTL_OFFSET        0x04 /*led ctl is in offset 05*/
#define LED_SWITCH_OFFSET     0x0C /*led switch is in offset 0x0E*/


#define TURN_ON_LEDS_CTRL                                             \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_CTL_OFFSET);              \
		*ptr ^= 0x00020000;                                   \
	}
#define SWITCH_LED_1                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00000100;                                   \
	}
#define SWITCH_LED_2                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00000200;                                   \
	}
#define SWITCH_LED_3                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00000400;                                   \
	}
#define SWITCH_LED_4                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00000800;                                   \
	}
#define SWITCH_LED_5                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00001000;                                   \
	}
#define SWITCH_LED_6                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00002000;                                   \
	}
#define SWITCH_LED_7                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00004000;                                   \
	}
#define SWITCH_LED_8                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00008000;                                   \
	}
/**
 * @Function turn_led_configuration;
 */
static inline void turn_led_configuration(void)
{
	TURN_ON_LEDS_CTRL;
}
/**
 * @Function turn_led_on;
 * @param[in] led_num bitmask to choose which leds to turn on (8 bits = 8 leds)
 */
static inline void turn_led_on(uint8_t led_num)
{
	if(led_num | 0x01)
		SWITCH_LED_1;
	if(led_num | 0x02)
		SWITCH_LED_2;
	if(led_num | 0x04)
		SWITCH_LED_3;
	if(led_num | 0x08)
		SWITCH_LED_4;
	if(led_num | 0x10)
		SWITCH_LED_5;
	if(led_num | 0x20)
		SWITCH_LED_6;
	if(led_num | 0x40)
		SWITCH_LED_7;
	if(led_num | 0x80)
		SWITCH_LED_8;
}

#endif /* __LED_H */

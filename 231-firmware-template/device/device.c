#include "device.h"

#include <stdio.h>
#include <stddef.h>

#include "pico/unique_id.h"
#include "pico/version.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sysinfo.h"

void device_get_info(device_info_t *info)
{
	
	char board_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
    pico_get_unique_board_id_string(board_id, sizeof(board_id));
	
	info->board_type = DEVICE_BOARD;
	for (int i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1; i++)
		info->board_id[i] = board_id[i];

	

	volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
		
    uint32_t id = *chip_id;
	
	info->manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> 
								SYSINFO_CHIP_ID_MANUFACTURER_LSB;
	
	info->part = (id & SYSINFO_CHIP_ID_PART_BITS) >> 
								SYSINFO_CHIP_ID_PART_LSB;
	
	info->revision = (id & SYSINFO_CHIP_ID_REVISION_BITS>> 
								SYSINFO_CHIP_ID_REVISION_LSB);	
}
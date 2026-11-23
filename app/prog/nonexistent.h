/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  VARIANT          NONEXISTENT
 ******************************************************************************/

#ifndef NONEXISTENT_H
#define NONEXISTENT_H

#define CRC32_SIZE	UINT32_C(0x4)
#define STACK_MAGIC_SIZE	UINT32_C(0x4)
#define APP_VARIANT
#define UC_VENDOR	CMS
#define PROC_FAMILY	CMS32M67XX
#define CMS32M67XX
#define CMS32M67
#define CMS32M6736
#define COMA_ERASE_BYTE	0x00u
#define RAM0START	0x20001000UL
#define RAM0TOP	0x20002FFFUL
#define ROM1START	0x6400UL
#define ROM1END	0x1FFFFUL
#define FLASH_SIZE	128UL
#define FWU_BOOTMANAGER_START_ADDRESS	UINT32_C(0x200)
#define FWU_BOOTMANAGER_END_ADDRESS	UINT32_C(0x10ff)
#define FWU_BOOTMANAGER_SIZE	UINT32_C(0xf00)
#define FWU_LRO_START_ADDRESS	UINT32_C(0x1100)
#define FWU_LRO_END_ADDRESS	UINT32_C(0x1bff)
#define FWU_LRO_SIZE	UINT32_C(0xb00)
#define FWU_PROGRAMMER_START_ADDRESS	UINT32_C(0x1c00)
#define FWU_PROGRAMMER_END_ADDRESS	UINT32_C(0x61ff)
#define FWU_PROGRAMMER_SIZE	UINT32_C(0x4600)
#define FWU_OTP_MEMORY_START_ADDRESS	UINT32_C(0x6200)
#define FWU_OTP_MEMORY_END_ADDRESS	UINT32_C(0x63ff)
#define FWU_OTP_MEMORY_SIZE	UINT32_C(0x200)
#define FWU_APP1_START_ADDRESS	UINT32_C(0x6400)
#define FWU_APP1_END_ADDRESS	UINT32_C(0x1ffff)
#define FWU_APP1_SIZE	UINT32_C(0x19c00)
#define RAM1_PHY_START	UINT32_C(0x20001000)
#define RAM1_PHY_END	UINT32_C(0x20002FFF)
#define RAM1_PHY_SIZE	UINT32_C(0x2000)
#define ROM1_PHY_START	UINT32_C(0x200)
#define ROM1_PHY_END	UINT32_C(0x1FFFF)
#define ROM3_PHY_START	UINT32_C(0x0)
#define ROM3_PHY_END	UINT32_C(0x3FF)
#define ROM3_PHY_SIZE	UINT32_C(0x400)
#define ROM1_START	UINT32_C(0x6400)
#define ROM1_END	UINT32_C(0x1FFFF)
#define ROM1_SIZE	UINT32_C(0x19C00)
#define ROM3_START	UINT32_C(0x0)
#define ROM3_END	UINT32_C(0x3FF)
#define ROM3_SIZE	UINT32_C(0x400)
#define RAM1_START	UINT32_C(0x20001000)
#define RAM1_END	UINT32_C(0x20002FFF)
#define RAM1_SIZE	UINT32_C(0x2000)
#define PAGE_SIZE	UINT32_C(0x200)
#define HEAPSIZE	UINT32_C(0x0)
#define STACKSIZE	UINT32_C(0x400)
#define FWU_SHARED_DATA_SIZE	UINT32_C(0x40)
#define UNINIT_DATA_SIZE	UINT32_C(0x40)
#define RAMCONT_EXCLUDED_DATA_SIZE	UINT32_C(0x0)
#define CORE_CLOCK	72U
#define CORE_CLOCK_72M
#define PCLOCK	72U
#define PCLOCK1	72U
#define VTOR_ALIGN	UINT32_C(0x200)
#define VECTOR_TABLE_SIZE	UINT32_C(0xC0)
#define FLASH_DRIVER_SIZE	UINT32_C(0x200)
#define DEBUG	0x00U
#define need_fixed_vectors
#define HUART0_RX_PA0
#define HUART0_TX_PA1
#define HUART0_RX_PIN	A0
#define HUART0_TX_PIN	A1
#define HUART0_USED
#define DBUS_DEFAULT_BAUDRATE	96U
#define CMS32M6736EGH48NB
#define MIN_WRITEBLOCK_SIZE	UINT32_C(0x4)
#define REMOTE_FIRMWARE_UPDATE
#define ECU_ID_COUNT	1U
#define APP_ID_NUMBER	1U
#define PROGRAMMER_ID_NUMBER	2U
#define CORTEX_M0_PLUS
#define NO_CORE_DEVIDER
#define ALIGNMENT	4
#define __LITTLE_ENDIAN_ADAPTATION
#define ROOT_REGION	UINT32_C(0x0)
#define TRASH_AREA_START	0x1000000
#define TRASH_AREA_SIZE	0x1000
#define RELOC_ADDR	0x0
#define FW_UID	UINT64_C(0)
#define VERSION_MAJOR	UINT16_C(0)
#define VERSION_MINOR	UINT16_C(0)
#define VERSION_REVISION	UINT16_C(0)

#endif /* NONEXISTENT_H */

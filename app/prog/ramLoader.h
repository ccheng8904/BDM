/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  VARIANT          RAMLOADER
 ******************************************************************************/

#ifndef RAMLOADER_H
#define RAMLOADER_H

#define CRC32_SIZE	UINT32_C(0x4)
#define STACK_MAGIC_SIZE	UINT32_C(0x4)
#define BUILT_IN_VARIANT
#define FWU_VARIANT
#define BOOTLOADER_VARIANT
#define UC_VENDOR	SH
#define PROC_FAMILY	SH30F9X
#define SH30_ARM
#define SH30F9X71
#define SH30F9071
#define COMA_ERASE_BYTE	0x00u
#define RAM0START	0x20000000UL
#define RAM0TOP	0x200007FFUL
#define ROM1START	0x20000800UL
#define ROM1END	0x20001FFFUL
#define FLASH_SIZE	256UL
#define I2C_BY_MCAL
#define MAL_RELEASE_KEY_DEF	0x5E
#define MAL_DEVELOP_KEY_DEF	0xDE
#define MAL_ENABLE_DEF	0xEA
#define MAL_IGNORE_ERROR_DEF	0x1E
#define MAL_DISABLE_CALC_DEF	0xDC
#define MAL_RESERVED_DEF	0xFF
#define MAL_ENABLE_INT_FLASH_DRIVER
#define MCAL_INCLUDED
#define MCAL_MDIO_INCLUDED
#define MCAL_MUART_INCLUDED
#define MCAL_MWDT_INCLUDED
#define MCAL_MCOM_INCLUDED
#define MCAL_MSUP_INCLUDED
#define RAM1_PHY_START	UINT32_C(0x20000000)
#define RAM1_PHY_END	UINT32_C(0x20003FFF)
#define RAM1_PHY_SIZE	UINT32_C(0x4000)
#define ROM1_PHY_START	UINT32_C(0x0)
#define ROM1_PHY_END	UINT32_C(0x3FFFF)
#define ROM3_PHY_START	UINT32_C(0xFFFE000)
#define ROM3_PHY_END	UINT32_C(0xFFFE3FF)
#define ROM3_PHY_SIZE	UINT32_C(0x400)
#define ROM1_START	UINT32_C(0x20000800)
#define ROM1_END	UINT32_C(0x20001FFF)
#define ROM1_SIZE	UINT32_C(0x1800)
#define ROM3_START	UINT32_C(0xFFFE000)
#define ROM3_END	UINT32_C(0xFFFE3FF)
#define ROM3_SIZE	UINT32_C(0x400)
#define RAM1_START	UINT32_C(0x20000000)
#define RAM1_END	UINT32_C(0x200007FF)
#define RAM1_SIZE	UINT32_C(0x800)
#define PAGE_SIZE	UINT32_C(0x400)
#define HEAPSIZE	UINT32_C(0x0)
#define STACKSIZE	UINT32_C(0x400)
#define FWU_SHARED_DATA_SIZE	UINT32_C(0x40)
#define UNINIT_DATA_SIZE	UINT32_C(0x40)
#define RAMCONT_EXCLUDED_DATA_SIZE	UINT32_C(0x0)
#define CORE_CLOCK	48U
#define CORE_CLOCK_48M
#define PCLOCK	24U
#define PCLOCK1	24U
#define VTOR_ALIGN	UINT32_C(0x200)
#define VECTOR_TABLE_SIZE	UINT32_C(0xC0)
#define FLASH_DRIVER_SIZE	UINT32_C(0x26C)
#define NDEBUG
#define DEBUG	0x00U
#define NO_ISR_SUPPORT
#define NO_RAM_INIT
#define BP2_UART_CHANNEL	0
#define BP2_MUART_CHANNEL	MUART0_RX_PB0_TX_PA15
#define HUART0_RX_PB0
#define HUART0_TX_PA15
#define HUART0_RX_PIN	B0
#define HUART0_TX_PIN	A15
#define DBUS_MUART_PORT	MUART0_RX_PB0_TX_PA15
#define MUART0_USED
#define DBM_BREAK_MDIO_CHANNEL	MDIO
#define DBM_IDLE_MDIO_CHANNEL	MDIOB0
#define DBM_IDLE_MEXTI_CHANNEL	MEXTIB0
#define HUART0_USED
#define DBUS_DEFAULT_BAUDRATE	96U
#define SH30F9071P064PR
#define MIN_WRITEBLOCK_SIZE	UINT32_C(0x4)
#define REMOTE_FIRMWARE_UPDATE
#define CORTEX_M0_PLUS
#define NO_CORE_DEVIDER
#define ALIGNMENT	4
#define __LITTLE_ENDIAN_ADAPTATION
#define ROOT_REGION	UINT32_C(0x0)
#define TRASH_AREA_START	0x1000000
#define TRASH_AREA_SIZE	0x1000
#define RELOC_ADDR	0x0
#define UID	UINT64_C(0)
#define VERSION_MAJOR	UINT16_C(0)
#define VERSION_MINOR	UINT16_C(0)
#define VERSION_REVISION	UINT16_C(0)

#endif /* RAMLOADER_H */

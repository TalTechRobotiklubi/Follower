#ifndef _EEPROM_H_
#define	_EEPROM_H_

#include "stm32f10x.h"

#define EEPROM_PAGE_SIZE	(uint16_t)0x400		//Page size 1Kb

#define FLASH_END_ADDR		(uint32_t)0x08007800
#define EEPROM_START_ADDR	((uint32_t)(FLASH_END_ADDR - 2*EEPROM_PAGE_SIZE))

#define EEPROM_PAGE0_BASE		((uint32_t)(EEPROM_START_ADDR + 0x000))
#define EEPROM_PAGE1_BASE		((uint32_t)(EEPROM_START_ADDR + EEPROM_PAGE_SIZE))

/* Page status definitions */
#define EEPROM_ERASED			((uint16_t)0xFFFF)	/* PAGE is empty */
#define EEPROM_RECEIVE_DATA		((uint16_t)0xEEEE)	/* PAGE is marked to receive data */
#define EEPROM_VALID_PAGE		((uint16_t)0x0000)	/* PAGE containing valid data */

/* Page full define */
enum FLASH_Status
{
	EEPROM_OK				= ((uint16_t)0x0000),
	EEPROM_OUT_SIZE			= ((uint16_t)0x0081),
	EEPROM_BAD_ADDRESS		= ((uint16_t)0x0082),
	EEPROM_BAD_FLASH		= ((uint16_t)0x0083),
	EEPROM_NOT_INIT			= ((uint16_t)0x0084),
	EEPROM_NO_VALID_PAGE	= ((uint16_t)0x00AB)
};

#define EEPROM_DEFAULT_DATA		0xFFFF

uint16_t EE_init(void);
//uint16_t init(uint32_t, uint32_t, uint32_t);

uint16_t format(void);

uint16_t erases(uint16_t *);
//uint16_t read(uint16_t address);
uint16_t read(uint16_t address, uint16_t *data);
uint16_t write(uint16_t address, uint16_t data);
uint16_t count(uint16_t *);
uint16_t maxcount(void);
uint16_t format(void);

FLASH_Status EE_ErasePage(uint32_t);

uint16_t EE_CheckPage(uint32_t, uint16_t);
uint16_t EE_CheckErasePage(uint32_t, uint16_t);
uint16_t EE_Format(void);
uint32_t EE_FindValidPage(void);
uint16_t EE_GetVariablesCount(uint32_t, uint16_t);
uint16_t EE_PageTransfer(uint32_t, uint32_t, uint16_t);
uint16_t EE_VerifyPageFullWriteVariable(uint16_t, uint16_t);

#endif
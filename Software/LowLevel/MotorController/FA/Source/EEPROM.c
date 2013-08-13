#include "EEPROM.h"

uint32_t PageBase0 = EEPROM_PAGE0_BASE;
uint32_t PageBase1 = EEPROM_PAGE1_BASE;
uint32_t PageSize = EEPROM_PAGE_SIZE;
uint16_t Status;

uint16_t EE_CheckPage(uint32_t pageBase, uint16_t status)
{
	uint32_t pageEnd = pageBase + (uint32_t)PageSize;

	// Page Status not EEPROM_ERASED and not a "state"
	if ((*(uint16_t*)pageBase) != EEPROM_ERASED && (*( uint16_t*)pageBase) != status)
		return EEPROM_BAD_FLASH;
	for(pageBase += 4; pageBase < pageEnd; pageBase += 4)
		if ((*(uint32_t*)pageBase) != 0xFFFFFFFF)	// Verify if slot is empty
			return EEPROM_BAD_FLASH;
	return EEPROM_OK;
}

FLASH_Status EE_ErasePage(uint32_t pageBase)
{
	FLASH_Status FlashStatus;
	uint16_t data = (*(uint16_t*)(pageBase));
	if ((data == EEPROM_ERASED) || (data == EEPROM_VALID_PAGE) || (data == EEPROM_RECEIVE_DATA))
		data = (*( uint16_t*)(pageBase + 2)) + 1;
	else
		data = 0;

	FlashStatus = FLASH_ErasePage(pageBase);
	if (FlashStatus == FLASH_COMPLETE)
		FlashStatus = FLASH_ProgramHalfWord(pageBase + 2, data);

	return FlashStatus;
}

uint16_t EE_CheckErasePage(uint32_t pageBase, uint16_t status)
{
	uint16_t FlashStatus;
	if (EE_CheckPage(pageBase, status) != EEPROM_OK)
	{
		FlashStatus = EE_ErasePage(pageBase);
		if (FlashStatus != FLASH_COMPLETE)
			return FlashStatus;
		return EE_CheckPage(pageBase, status);
	}
	return EEPROM_OK;
}

uint32_t EE_FindValidPage(void)
{
	uint16_t status0 = (*( uint16_t*)PageBase0);		// Get Page0 actual status
	uint16_t status1 = (*( uint16_t*)PageBase1);		// Get Page1 actual status

	if (status0 == EEPROM_VALID_PAGE && status1 == EEPROM_ERASED)
		return PageBase0;
	if (status1 == EEPROM_VALID_PAGE && status0 == EEPROM_ERASED)
		return PageBase1;

	return 0;
}

uint16_t EE_GetVariablesCount(uint32_t pageBase, uint16_t skipAddress)
{
	uint16_t varAddress, nextAddress;
	uint32_t idx;
	uint32_t pageEnd = pageBase + (uint32_t)PageSize;
	uint16_t count = 0;

	for (pageBase += 6; pageBase < pageEnd; pageBase += 4)
	{
		varAddress = (*( uint16_t*)pageBase);
		if (varAddress == 0xFFFF || varAddress == skipAddress)
			continue;

		count++;
		for(idx = pageBase + 4; idx < pageEnd; idx += 4)
		{
			nextAddress = (*( uint16_t*)idx);
			if (nextAddress == varAddress)
			{
				count--;
				break;
			}
		}
	}
	return count;
}

uint16_t EE_PageTransfer(uint32_t newPage, uint32_t oldPage, uint16_t SkipAddress)
{
	uint32_t oldEnd, newEnd;
	uint32_t oldIdx, newIdx, idx;
	uint16_t address, data, found;
	FLASH_Status FlashStatus;

	// Transfer process: transfer variables from old to the new active page
	newEnd = newPage + ((uint32_t)PageSize);

	// Find first free element in new page
	for (newIdx = newPage + 4; newIdx < newEnd; newIdx += 4)
		if ((*( uint32_t*)newIdx) == 0xFFFFFFFF)	// Verify if element
			break;									//  contents are 0xFFFFFFFF
	if (newIdx >= newEnd)
		return EEPROM_OUT_SIZE;

	oldEnd = oldPage + 4;
	oldIdx = oldPage + (uint32_t)(PageSize - 2);

	for (; oldIdx > oldEnd; oldIdx -= 4)
	{
		address = *( uint16_t*)oldIdx;
		if (address == 0xFFFF || address == SkipAddress)
			continue;						// it's means that power off after write data

		found = 0;
		for (idx = newPage + 6; idx < newIdx; idx += 4)
			if ((*( uint16_t*)(idx)) == address)
			{
				found = 1;
				break;
			}

		if (found)
			continue;

		if (newIdx < newEnd)
		{
			data = (*( uint16_t*)(oldIdx - 2));

			FlashStatus = FLASH_ProgramHalfWord(newIdx, data);
			if (FlashStatus != FLASH_COMPLETE)
				return FlashStatus;

			FlashStatus = FLASH_ProgramHalfWord(newIdx + 2, address);
			if (FlashStatus != FLASH_COMPLETE)
				return FlashStatus;

			newIdx += 4;
		}
		else
			return EEPROM_OUT_SIZE;
	}

	// Erase the old Page: Set old Page status to EEPROM_EEPROM_ERASED status
	data = EE_CheckErasePage(oldPage, EEPROM_ERASED);
	if (data != EEPROM_OK)
		return data;

	// Set new Page status
	FlashStatus = FLASH_ProgramHalfWord(newPage, EEPROM_VALID_PAGE);
	if (FlashStatus != FLASH_COMPLETE)
		return FlashStatus;

	return EEPROM_OK;
}

uint16_t EE_VerifyPageFullWriteVariable(uint16_t Address, uint16_t Data)
{
	FLASH_Status FlashStatus;
	uint32_t idx, pageBase, pageEnd, newPage;
	uint16_t count;

	// Get valid Page for write operation
	pageBase = EE_FindValidPage();
	if (pageBase == 0)
		return  EEPROM_NO_VALID_PAGE;

	// Get the valid Page end Address
	pageEnd = pageBase + PageSize;			// Set end of page

	for (idx = pageEnd - 2; idx > pageBase; idx -= 4)
	{
		if ((*( uint16_t*)idx) == Address)		// Find last value for address
		{
			count = (*( uint16_t*)(idx - 2));	// Read last data
			if (count == Data)
				return EEPROM_OK;
			if (count == 0xFFFF)
			{
				FlashStatus = FLASH_ProgramHalfWord(idx - 2, Data);	// Set variable data
				if (FlashStatus == FLASH_COMPLETE)
					return EEPROM_OK;
			}
			break;
		}
	}

	// Check each active page address starting from begining
	for (idx = pageBase + 4; idx < pageEnd; idx += 4)
		if ((*( uint32_t*)idx) == 0xFFFFFFFF)				// Verify if element 
		{													//  contents are 0xFFFFFFFF
			FlashStatus = FLASH_ProgramHalfWord(idx, Data);	// Set variable data
			if (FlashStatus != FLASH_COMPLETE)
				return FlashStatus;
			FlashStatus = FLASH_ProgramHalfWord(idx + 2, Address);	// Set variable virtual address
			if (FlashStatus != FLASH_COMPLETE)
				return FlashStatus;
			return EEPROM_OK;
		}

	// Empty slot not found, need page transfer
	// Calculate unique variables in page
	count = EE_GetVariablesCount(pageBase, Address) + 1;
	if (count >= (PageSize / 4 - 1))
		return EEPROM_OUT_SIZE;

	if (pageBase == PageBase1)
		newPage = PageBase0;		// New page address where variable will be moved to
	else
		newPage = PageBase1;

	// Set the new Page status to RECEIVE_DATA status
	FlashStatus = FLASH_ProgramHalfWord(newPage, EEPROM_RECEIVE_DATA);
	if (FlashStatus != FLASH_COMPLETE)
		return FlashStatus;

	// Write the variable passed as parameter in the new active page
	FlashStatus = FLASH_ProgramHalfWord(newPage + 4, Data);
	if (FlashStatus != FLASH_COMPLETE)
		return FlashStatus;

	FlashStatus = FLASH_ProgramHalfWord(newPage + 6, Address);
	if (FlashStatus != FLASH_COMPLETE)
		return FlashStatus;

	return EE_PageTransfer(newPage, pageBase, Address);
}

uint16_t format(void)
{
	uint16_t status;
	FLASH_Status FlashStatus;

	FLASH_Unlock();

	// Erase Page0
	status = EE_CheckErasePage(PageBase0, EEPROM_VALID_PAGE);
	if (status != EEPROM_OK)
		return status;
	if ((*(uint16_t*)PageBase0) == EEPROM_ERASED)
	{
		// Set Page0 as valid page: Write VALID_PAGE at Page0 base address
		FlashStatus = FLASH_ProgramHalfWord(PageBase0, EEPROM_VALID_PAGE);
		if (FlashStatus != FLASH_COMPLETE)
			return FlashStatus;
	}
	// Erase Page1
	return EE_CheckErasePage(PageBase1, EEPROM_ERASED);
}
uint16_t EE_init(void)
{
	uint16_t status0, status1;
	FLASH_Status FlashStatus;

	FLASH_Unlock();
	Status = EEPROM_NO_VALID_PAGE;

	status0 = (*( uint16_t *)PageBase0);
	status1 = (*( uint16_t *)PageBase1);

	switch (status0)
	{
/*
		Page0				Page1
		-----				-----
	EEPROM_ERASED		EEPROM_VALID_PAGE			Page1 valid, Page0 erased
						EEPROM_RECEIVE_DATA			Page1 need set to valid, Page0 erased
						EEPROM_ERASED				make EE_Format
						any							Error: EEPROM_NO_VALID_PAGE
*/
	case EEPROM_ERASED:
		if (status1 == EEPROM_VALID_PAGE)			// Page0 erased, Page1 valid
			Status = EE_CheckErasePage(PageBase0, EEPROM_ERASED);
		else if (status1 == EEPROM_RECEIVE_DATA)	// Page0 erased, Page1 receive
		{
			FlashStatus = FLASH_ProgramHalfWord(PageBase1, EEPROM_VALID_PAGE);
			if (FlashStatus != FLASH_COMPLETE)
				Status = FlashStatus;
			else
				Status = EE_CheckErasePage(PageBase0, EEPROM_ERASED);
		}
		else if (status1 == EEPROM_ERASED)			// Both in erased state so format EEPROM
			Status = format();
		break;
/*
		Page0				Page1
		-----				-----
	EEPROM_RECEIVE_DATA	EEPROM_VALID_PAGE			Transfer Page1 to Page0
						EEPROM_ERASED				Page0 need set to valid, Page1 erased
						any							EEPROM_NO_VALID_PAGE
*/
    case EEPROM_RECEIVE_DATA:
		if (status1 == EEPROM_VALID_PAGE)			// Page0 receive, Page1 valid
			Status = EE_PageTransfer(PageBase0, PageBase1, 0xFFFF);
		else if (status1 == EEPROM_ERASED)			// Page0 receive, Page1 erased
		{
			Status = EE_CheckErasePage(PageBase1, EEPROM_ERASED);
			if (Status == EEPROM_OK)
			{
				FlashStatus = FLASH_ProgramHalfWord(PageBase0, EEPROM_VALID_PAGE);
				if (FlashStatus != FLASH_COMPLETE)
					Status = FlashStatus;
				else
					Status = EEPROM_OK;
			}
		}
		break;
/*
		Page0				Page1
		-----				-----
	EEPROM_VALID_PAGE	EEPROM_VALID_PAGE			Error: EEPROM_NO_VALID_PAGE
						EEPROM_RECEIVE_DATA			Transfer Page0 to Page1
						any							Page0 valid, Page1 erased
*/
	case EEPROM_VALID_PAGE:
		if (status1 == EEPROM_VALID_PAGE)			// Both pages valid
			Status = EEPROM_NO_VALID_PAGE;
		else if (status1 == EEPROM_RECEIVE_DATA)
			Status = EE_PageTransfer(PageBase1, PageBase0, 0xFFFF);
		else
			Status = EE_CheckErasePage(PageBase1, EEPROM_ERASED);
		break;
/*
		Page0				Page1
		-----				-----
		any				EEPROM_VALID_PAGE			Page1 valid, Page0 erased
						EEPROM_RECEIVE_DATA			Page1 valid, Page0 erased
						any							EEPROM_NO_VALID_PAGE
*/
	default:
		if (status1 == EEPROM_VALID_PAGE)
			Status = EE_CheckErasePage(PageBase0, EEPROM_ERASED);	// Check/Erase Page0
		else if (status1 == EEPROM_RECEIVE_DATA)
		{
			FlashStatus = FLASH_ProgramHalfWord(PageBase1, EEPROM_VALID_PAGE);
			if (FlashStatus != FLASH_COMPLETE)
				Status = FlashStatus;
			else
				Status = EE_CheckErasePage(PageBase0, EEPROM_ERASED);
		}
		break;
	}
	return Status;
}


uint16_t erases(uint16_t *Erases)
{
	uint32_t pageBase;
	// Get active Page for read operation
	pageBase = EE_FindValidPage();
	if (pageBase == 0)
		return  EEPROM_NO_VALID_PAGE;

	*Erases = (*( uint16_t*)pageBase+2);
	return EEPROM_OK;
}

//uint16_t read(uint16_t Address)
//{
//	uint16_t data;
//	read(Address, &data);
//	return data;
//}

uint16_t read(uint16_t Address, uint16_t *Data)
{
	uint32_t pageBase, pageEnd;

	// Set default data (empty EEPROM)
	*Data = EEPROM_DEFAULT_DATA;

	// Get active Page for read operation
	pageBase = EE_FindValidPage();
	if (pageBase == 0)
		return  EEPROM_NO_VALID_PAGE;

	// Get the valid Page end Address
	pageEnd = pageBase + ((uint32_t)(PageSize - 2));
	
	// Check each active page address starting from end
	for (pageBase += 6; pageEnd >= pageBase; pageEnd -= 4)
		if ((*( uint16_t*)pageEnd) == Address)		// Compare the read address with the virtual address
		{
			*Data = (*( uint16_t*)(pageEnd - 2));		// Get content of Address-2 which is variable value
			return EEPROM_OK;
		}

	// Return ReadStatus value: (0: variable exist, 1: variable doesn't exist)
	return EEPROM_BAD_ADDRESS;
}

uint16_t write(uint16_t Address, uint16_t Data)
{
	if (Address == 0xFFFF)
		return EEPROM_BAD_ADDRESS;

	// Write the variable virtual address and value in the EEPROM
	uint16_t status = EE_VerifyPageFullWriteVariable(Address, Data);
	return status;
}

uint16_t count(uint16_t *Count)
{
	// Get valid Page for write operation
	uint32_t pageBase = EE_FindValidPage();
	if (pageBase == 0)
		return EEPROM_NO_VALID_PAGE;	// No valid page, return max. numbers

	*Count = EE_GetVariablesCount(pageBase, 0xFFFF);
	return EEPROM_OK;
}

uint16_t maxcount(void)
{
	return ((PageSize / 4)-1);
}
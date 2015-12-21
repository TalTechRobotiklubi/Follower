/*
 * usb.c
 *
 *  Created on: Dec 10, 2015
 *      Author: peeter
 */

#include "usb.h"

#include "tm_stm32f4_usb_vcp.h"

#include "DataLayer.h"

void USB_init()
{
	TM_USB_VCP_Init();
}

void USB_task()
{
	/* USB configured OK, drivers OK */
	if (TM_USB_VCP_GetStatus() == TM_USB_VCP_CONNECTED)
	{
		uint8_t c;
		/* If something arrived at VCP */
		if (TM_USB_VCP_Getc(&c) == TM_USB_VCP_DATA_OK)
		{
			/* Return data back */
			TM_USB_VCP_Putc(c);
			int8_t servo1 = (int8_t)c;
			DL_setData(DLParamCameraRequestXDegree, &servo1);
		}
	}
}

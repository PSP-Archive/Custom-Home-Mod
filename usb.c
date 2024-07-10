//Thanks neur0n!!

#include <pspkernel.h>
#include <pspusb.h>
#include <pspusbstor.h>
#include <pspusbdevice.h>
#include <pspmodulemgr.h>
#include <pspsdk.h>

#include "common.h"

typedef struct
{
	char *path;
	int uid;
}UsbModuleList;

int StopUnloadModule(SceUID id)
{
	SceUID r = sceKernelStopModule(id ,0 ,NULL , NULL, NULL);
	if(r < 0) return r;
	return sceKernelUnloadModule(id);
}

static UsbModuleList usb_list[] = {
	{"flash0:/kd/semawm.prx"		, -1 },
	{"flash0:/kd/usbstor.prx"		, -1 },
	{"flash0:/kd/usbstormgr.prx"	, -1 },
	{"flash0:/kd/usbstorms.prx"		, -1 },
	{"flash0:/kd/usbstoreflash.prx"	, -1 },
	{"flash0:/kd/usbstorboot.prx"	, -1 },
};

#define USB_MODULE_CNT	(sizeof(usb_list)/sizeof(UsbModuleList))
void LoadUsbModules()
{
	int i;
	for(i=0;i<USB_MODULE_CNT;i++)
	{
		usb_list[i].uid = LoadStartModule( usb_list[i].path );
	}
}

void UnoadUsbModules()
{
	int i;
	for(i= (USB_MODULE_CNT - 1); i< 0 ;i--)
	{
		if( usb_list[i].uid >= 0)
		{
			StopUnloadModule(usb_list[i].uid);
			usb_list[i].uid = -1;
		}
	}
}
int connect_usb()
{
	static int connect_flag = 0;

	if( connect_flag )
	{
		sceUsbDeactivate(0);
		//Flush MS and SS to avoid corrupted files
		sceIoDevctl("mscmhc0:", 0x0240D81E, NULL, 0, NULL, 0 );
		if (model == 4 ) { sceIoDevctl("mscmhcemu0:", 0x0240D81E, NULL, 0, NULL, 0 ); }

		sceUsbStop(PSP_USBSTOR_DRIVERNAME, 0, 0);	
		sceUsbStop(PSP_USBBUS_DRIVERNAME, 0, 0);

		UnoadUsbModules();
	}
	else
	{
		LoadUsbModules();

		sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0);
		sceUsbStart(PSP_USBSTOR_DRIVERNAME, 0, 0);
//			sceUsbstorBootSetCapacity(0x800000);
		switch (model){
			case 0: //PSP 1000
				sceUsbActivate(0x1c8);
				break;
			case 4: //PSP Go
				sceUsbActivate(0x381);
				break;
			default: //PSP Slim; Brite and Street unknown
				sceUsbActivate(0x2D2);
		}
	}

	// select new
	connect_flag = 1- connect_flag;

	return connect_flag;
}


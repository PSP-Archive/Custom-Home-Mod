
/*
 * PSPLINK
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPLINK root for details.
 *
 * bitmap.c - PSPLINK kernel module bitmap code
 *
 * Copyright (c) 2005 James F <tyranid@gmail.com>
 *
 * $HeadURL: svn://svn.pspdev.org/psp/trunk/psplinkusb/psplink/bitmap.c $
 * $Id: bitmap.c 2026 2006-10-14 13:09:48Z tyranid $
 */

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspsuspend.h>
#include <pspinit.h>
#include <pspsysmem_kernel.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "pict.h"


static int fixed_write(int fd, void *data, int len)
{
	int writelen = 0;
	
	while(writelen < len)
	{
		int ret;
		
		ret = sceIoWrite(fd, data + writelen, len - writelen);
		if(ret <= 0)
		{
			writelen = -1;
			break;
		}
		writelen += ret;
	}
	
	return writelen;
}

int write_8888_data(void *frame, void *pixel_data)
{
	uint8_t *line;
	uint8_t *p;
	int i;
	int h;
	
	line = pixel_data;
	for(h = 271; h >= 0; h--)
	{
		p = frame + (h*512*4);
		for(i = 0; i < 480; i++)
		{
			line[(i*3) + 2] = p[i*4];
			line[(i*3) + 1] = p[(i*4) + 1];
			line[(i*3) + 0] = p[(i*4) + 2];
		}
		line += 480 * 3;
	}
	
	return 0;
}

int write_5551_data(void *frame, void *pixel_data)
{
	uint8_t *line;
	uint16_t *p;
	int i;
	int h;
	
	line = pixel_data;
	for(h = 271; h >= 0; h--)
	{
		p = frame;
		p += (h * 512);
		for(i = 0; i < 480; i++)
		{
			line[(i*3) + 2] = (p[i] & 0x1F) << 3;
			line[(i*3) + 1] = ((p[i] >> 5) & 0x1F) << 3;
			line[(i*3) + 0] = ((p[i] >> 10) & 0x1F) << 3;
		}
		line += 480*3;
	}
	
	return 0;
}

int write_565_data(void *frame, void *pixel_data)
{
	uint8_t *line;
	uint16_t *p;
	int i;
	int h;
	
	line = pixel_data;
	for(h = 271; h >= 0; h--)
	{
		p = frame;
		p += (h * 512);
		for(i = 0; i < 480; i++)
		{
			line[(i*3) + 2] = (p[i] & 0x1F) << 3;
			line[(i*3) + 1] = ((p[i] >> 5) & 0x3F) << 2;
			line[(i*3) + 0] = ((p[i] >> 11) & 0x1F) << 3;
		}
		line += 480*3;
	}
	
	return 0;
}

int write_4444_data(void *frame, void *pixel_data)
{
	uint8_t *line;
	uint16_t *p;
	int i;
	int h;
	
	line = pixel_data;
	for(h = 271; h >= 0; h--)
	{
		p = frame;
		p += (h * 512);
		for(i = 0; i < 480; i++)
		{
			line[(i*3) + 2] = (p[i] & 0xF) << 4;
			line[(i*3) + 1] = ((p[i] >> 4) & 0xF) << 4;
			line[(i*3) + 0] = ((p[i] >> 8) & 0xF) << 4;
		}
		line += 480*3;
	}
	
	return 0;
}

int bitmapWrite(void *frame_addr, void *tmp_buf, int format, const char *file)
{
	BitmapHeader *bmp;
	void *pixel_data = tmp_buf + sizeof(BitmapHeader);
	int fd;
	
	bmp = (BitmapHeader *) tmp_buf;
	memset(bmp, 0, sizeof(BitmapHeader));
	memcpy(bmp->id, BMP_ID, sizeof(bmp->id));
	bmp->filesize = 480*272*3 + sizeof(BitmapHeader);
	bmp->offset = sizeof(BitmapHeader);
	bmp->headsize = 0x28;
	bmp->width = 480;
	bmp->height = 272;
	bmp->planes = 1;
	bmp->bpp = 24;
	bmp->bitmapsize = 480*272*3;
	bmp->hres = 2834;
	bmp->vres = 2834;
	
	switch(format)
	{
		case PSP_DISPLAY_PIXEL_FORMAT_565: write_565_data(frame_addr, pixel_data);
			break;
		case PSP_DISPLAY_PIXEL_FORMAT_5551: write_5551_data(frame_addr, pixel_data); 
			break;
		case PSP_DISPLAY_PIXEL_FORMAT_4444: write_4444_data(frame_addr, pixel_data);
			break;
		case PSP_DISPLAY_PIXEL_FORMAT_8888: write_8888_data(frame_addr, pixel_data);
			break;
	};
	
	if((fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777)) >= 0) {
		fixed_write(fd, tmp_buf, bmp->filesize);
		sceIoClose(fd);
		return 0;
	}
	return -1;
}


//---thanks to prxshot v0.4.0---

int takeScreenShot(const char *path)
{
	void *frame_addr;
	int frame_width, pixel_format;
	unsigned int ptr;
	int ret = -1;

	void *mem = mem_alloc(BMP_SIZE);
	if(mem != NULL) {
		sceDisplayWaitVblankStart();
		if(sceDisplayGetFrameBuf(&frame_addr, &frame_width, &pixel_format, PSP_DISPLAY_SETBUF_NEXTFRAME) >= 0 && frame_addr) {
			ptr = (unsigned int)frame_addr;
			ptr |= ptr & 0x80000000 ?  0xA0000000 : 0x40000000;
			bitmapWrite((void *)ptr, mem, pixel_format, path);
			ret = 0;
		}
		mem_free(mem);
	}
	return ret;
}

/*
int takeScreenShotForBuf(PictInfo *data, libm_draw_info *dinfo)
{
	u32 *vram32;
	u16 *vram16, *raw16;
	int i;

	data->memid = -1;
	sceDisplayWaitVblankStart();
	libmInitBuffers(0, PSP_DISPLAY_SETBUF_NEXTFRAME, dinfo);

	data->data.size = dinfo->vinfo->lineWidth * dinfo->vinfo->height * 2;
	data->data.width = dinfo->vinfo->width;
	data->data.height = dinfo->vinfo->height;
	data->data.raw = allocScreenShotBufferMemory(data->data.size, &data->memid);
	if( data->data.raw == NULL )
	{
		return -1;
	}

	memset(data->data.raw, 0, data->data.size);
	raw16 = (u16 *)data->data.raw;

	
	switch(dinfo->vinfo->format)
	{
		case PSP_DISPLAY_PIXEL_FORMAT_5551:
		{
			vram16 = (u16 *)dinfo->vinfo->buffer;

			for(i=0; i< data->data.size; i++)
			{
				raw16[i] = vram16[i];
			}
			break;
		}
		case PSP_DISPLAY_PIXEL_FORMAT_8888:
		{
			vram32 = (u32 *)dinfo->vinfo->buffer;
			
			for(i=0; i< data->data.size; i++)
			{
				raw16[i] = libmConvert8888_5551(vram32[i]);
			}
			break;
		}
		case PSP_DISPLAY_PIXEL_FORMAT_565:
		{
			vram16 = (u16 *)dinfo->vinfo->buffer;

			for(i=0; i< data->data.size; i++)
			{
				raw16[i] = libmConvert5650_5551(vram16[i]);
			}
			break;
		}
		case PSP_DISPLAY_PIXEL_FORMAT_4444:
		{
			vram16 = (u16 *)dinfo->vinfo->buffer;

			for(i=0; i< data->data.size; i++)
			{
				raw16[i] = libmConvert4444_5551(vram16[i]);
			}
			break;
		}
	}
	return 0;
}

void drawPictForBuf(PictInfo *data, libm_draw_info *dinfo)
{
	u32 *vram32;
	u16 *vram16, *raw16 = (u16 *)data->data.raw;
	int i;
	
	sceDisplayWaitVblankStart();
	libmInitBuffers(0, PSP_DISPLAY_SETBUF_NEXTFRAME, dinfo);

	switch(dinfo->vinfo->format)
	{
		case PSP_DISPLAY_PIXEL_FORMAT_5551:
		{
			vram16 = (u16 *)dinfo->vinfo->buffer;

			for(i=0; i< data->data.size; i++)
			{
				vram16[i] = raw16[i];
			}
			break;
		}
		case PSP_DISPLAY_PIXEL_FORMAT_8888:
		{
			vram32 = (u32 *)dinfo->vinfo->buffer;
			
			for(i=0; i< data->data.size; i++)
			{
				vram32[i] = libmConvert5551_8888(raw16[i]);
			}
			break;
		}
		case PSP_DISPLAY_PIXEL_FORMAT_565:
		{
			vram16 = (u16 *)dinfo->vinfo->buffer;

			for(i=0; i< data->data.size; i++)
			{
				vram16[i] = libmConvert5551_5650(raw16[i]);
			}
			break;
		}
		case PSP_DISPLAY_PIXEL_FORMAT_4444:
		{
			vram16 = (u16 *)dinfo->vinfo->buffer;

			for(i=0; i< data->data.size; i++)
			{
				vram16[i] = libmConvert5551_4444(raw16[i]);
			}
			break;
		}
	}
	return;
}
*/

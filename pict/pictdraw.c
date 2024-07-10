//Thanks hiroi01

#include <pspkernel.h>
#include <pspdisplay.h>
#include "pict.h"

//thanks to libmenu by maxem


inline u32 libmConvert5551_5650( u32 src )
{
	return libmConvert8888_5650(libmConvert5551_8888(src));
}

inline u32 libmConvert5551_4444( u32 src )
{
	return libmConvert8888_4444(libmConvert5551_8888(src));
}

inline u32 libmConvert5650_5551( u32 src )
{
	return libmConvert8888_5551(libmConvert5650_8888(src));
}

inline u32 libmConvert4444_5551( u32 src )
{
	return libmConvert8888_5551(libmConvert4444_8888(src));
}


void drawPict5551(int x, int y, PictData *data, libm_draw_info *dinfo)
{
	u32 *vram32, *vram32_ptr;
	u16 *vram16, *vram16_ptr;
	int i, j, count= 0;
	u16 *raw16 = (u16 *)data->raw;
	
	switch(dinfo->vinfo->format)
	{
		case PSP_DISPLAY_PIXEL_FORMAT_5551:
		{
			vram16 = (u16 *)dinfo->vinfo->buffer + x + y * dinfo->vinfo->lineWidth;
			
			for(j = 0; j < data->height; j++)
			{
				vram16_ptr = vram16;
				
				for(i = 0; i < data->width; i++, vram16_ptr++)
					*vram16_ptr = raw16[count++];
				
				vram16 += dinfo->vinfo->lineWidth;
			}
			
			
			break;
		}			
		case PSP_DISPLAY_PIXEL_FORMAT_8888:
		{
			vram32 = (u32 *)dinfo->vinfo->buffer + x + y * dinfo->vinfo->lineWidth;
			
			for(j = 0; j < data->height; j++)
			{
				vram32_ptr = vram32;
				
				for(i = 0; i < data->width; i++, vram32_ptr++)
					*vram32_ptr = libmConvert5551_8888(raw16[count++]);
				
				vram32 += dinfo->vinfo->lineWidth;
			}
			
			break;
		}
		case PSP_DISPLAY_PIXEL_FORMAT_565:
		{
			vram16 = (u16 *)dinfo->vinfo->buffer + x + y * dinfo->vinfo->lineWidth;
			
			for(j = 0; j < data->height; j++)
			{
				vram16_ptr = vram16;
				
				for(i = 0; i < data->width; i++, vram16_ptr++){
					*vram16_ptr = libmConvert5551_5650(raw16[count++]);
				}
				vram16 += dinfo->vinfo->lineWidth;
			}
			
			
			break;
		}
		case PSP_DISPLAY_PIXEL_FORMAT_4444:
		{
			vram16 = (u16 *)dinfo->vinfo->buffer + x + y * dinfo->vinfo->lineWidth;
			
			for(j = 0; j < data->height; j++)
			{
				vram16_ptr = vram16;
				
				for(i = 0; i < data->width; i++, vram16_ptr++)
					*vram16_ptr = libmConvert5551_4444(raw16[count++]);
				
				vram16 += dinfo->vinfo->lineWidth;
			}
			
			
			break;
		}
	}
	return;
	
}


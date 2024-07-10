#include <pspkernel.h>
#include <string.h>
#include "pict.h"


// function
/*
static __inline u16 convert_8888_to_565(u32 color)
{
	int r, g, b;

	b = (color >> 19) & 0x1F;
	g = (color >> 10) & 0x3F;
	r = (color >>  3) & 0x1F;

	return r | (g << 5) | (b << 11);
}
*/
__inline u16 convert_8888_to_5551(u32 color)
{
	int r, g, b, a;

	a = (color >> 24) ? 0x8000 : 0;
	b = (color >> 19) & 0x1F;
	g = (color >> 11) & 0x1F;
	r = (color >>  3) & 0x1F;

	return a | r | (g << 5) | (b << 10);
}
/*
static __inline u16 convert_8888_to_4444(u32 color)
{
	int r, g, b, a;

	a = (color >> 28) & 0xF;
	b = (color >> 20) & 0xF;
	g = (color >> 12) & 0xF;
	r = (color >>  4) & 0xF;

	return (a << 12) | r | (g << 4) | (b << 8);
}

__inline u16 convert_u32_to_u16(u32 color, int pixelformat)
{
	switch(pixelformat)
	{
		case PSP_DISPLAY_PIXEL_FORMAT_565:		return convert_8888_to_565(color);
		case PSP_DISPLAY_PIXEL_FORMAT_5551:		return convert_8888_to_5551(color);
		case PSP_DISPLAY_PIXEL_FORMAT_4444:		return convert_8888_to_4444(color);
	}

	return (u16)color;
}
*/

int convertAndRescaleTo5551(void *inRaw, u32 inWidth, u32 inHeight, void *outRaw, u32 outWidth, u32 outHeight)
{
	u8 *line;
	u16 *p;
	int i;
	int h;
	int offset;
	double xpos, ypos;
    double hokanX = (double) inWidth / outWidth;
    double hokanY = (double) inHeight / outHeight;
	
	p = outRaw;
	line = inRaw + (inWidth  * (inHeight - 1) * 3) ;
	
	ypos = 0.0;
	for(h = 0; h < outHeight; h++){
		xpos = 0.0;
		for(i = 0; i < outWidth; i++){
			offset = (int)( ((xpos) - (ypos * inWidth)) * 3 );
			*p++ =	convert_8888_to_5551(
										 0xff000000 |
										 (line[offset + 0] << 16) |
										 (line[offset + 1] << 8) | 
										 (line[offset + 2])
										 );
			xpos += hokanX;
		}
		ypos += hokanY;
	}
	return 0;
}
/*
int read_8888_data(void *inData, void *outData, u32 width, u32 height)
{
	u8 *line;
	u32 *p;
	int i;
	int h;
	int unk = 0;

	// ‰æ–Êî•ñ‚ÌŽæ“¾
	sceDisplayGetMode(&unk, &canvas.width, &canvas.height);
	sceDisplayGetFrameBuf((void *)&canvas.buffer, &canvas.linewidth, &canvas.pixelformat, 1);
	
	p = outData;
	line = inData + ( (width * 3) * (height - 1) ) ;
	for(h = height; h >= 0; h--){
		for(i = 0; i < width; i++){
			switch(canvas.pixelformat)
			{
				case PSP_DISPLAY_PIXEL_FORMAT_565:
					*p++ =	libmConvert5650_5551(0xff000000 | (line[(i*3) + 0] << 16) | (line[(i*3) + 1] << 8) | (line[(i*3) + 2]));
					break;
				case PSP_DISPLAY_PIXEL_FORMAT_5551:
					*p++ =	0xff000000 | (line[(i*3) + 0] << 16) | (line[(i*3) + 1] << 8) | (line[(i*3) + 2]);
					break;
				case PSP_DISPLAY_PIXEL_FORMAT_4444:
					*p++ =	libmConvert4444_5551(0xff000000 | (line[(i*3) + 0] << 16) | (line[(i*3) + 1] << 8) | (line[(i*3) + 2]));
					break;
				case PSP_DISPLAY_PIXEL_FORMAT_8888:
					*p++ =	convert_8888_to_5551(0xff000000 | (line[(i*3) + 0] << 16) | (line[(i*3) + 1] << 8) | (line[(i*3) + 2]));
					break;
			}
		}
		line -= width * 3;
	}
	
	return 0;
}
*/

int loadBmp(const char *path, PictData *data)
{
	SceUID fd = -1;
	BitmapHeader header;
	int ret = 0;
	void *buf = NULL;
	
	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
	if( fd < 0 ){
		ret = -1;
		goto EXIT;
	}
	
	sceIoLseek(fd, 0, PSP_SEEK_SET);
	ret = sceIoRead(fd, &header, sizeof(BitmapHeader));
	if( ret != sizeof(BitmapHeader) ){
		ret = -2;
		goto EXIT;
	}
	
	buf = mem_alloc(header.bitmapsize);
	if(buf == NULL) {
		ret = -3;
		goto EXIT;
	}
	
	ret = sceIoRead(fd, buf, header.bitmapsize);
	if( ret != header.bitmapsize ){
		ret = -4;
		goto EXIT;
	}
	
	convertAndRescaleTo5551(buf, header.width, header.height, data->raw, data->width, data->height);
	
	
	ret = 0;
	
EXIT:
	if( !(fd < 0) ){
		sceIoClose(fd);
	}
	if( buf != NULL ){
		mem_free(buf);
	}
	
	return ret;
	
}




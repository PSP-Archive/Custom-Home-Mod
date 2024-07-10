#ifndef __BITMAP_H__
#define __BITMAP_H__

#define BMP_ID "BM"

typedef struct BitmapHeader_
{
	char id[2];
	u32 filesize;
	u32 reserved;
	u32 offset;
	u32 headsize;
	u32 width;
	u32 height;
	uint16_t planes;
	uint16_t bpp;
	u32 comp;
	u32 bitmapsize;
	u32 hres;
	u32 vres;
	u32 colors;
	u32 impcolors;
} __attribute__((packed)) BitmapHeader;

#endif

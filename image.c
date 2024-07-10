#include <pspkernel.h>
#include <string.h>
#include "pict/bitmap.h"
#include "memory.h"

#define READ_BMP_LINE_SIZE (480*3*8)
#define VRAM_SIZE (512*272*2)

int init_image(char *path, u16 *vram){
	int i, j, h;
	SceUID fd = -1;
	BitmapHeader header;
	int ret = 0;
	unsigned char *p, *buf = NULL;
	u16 *vram_p = vram;
	
	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
	if( fd < 0 ){
		ret = -1;
		goto EXIT;
	}
	
	sceIoLseek(fd, 0, PSP_SEEK_SET);
	ret = sceIoRead(fd, &header, sizeof(BitmapHeader));
	if( ret != sizeof(BitmapHeader) || header.width != 480 || header.height != 272 || header.bpp != 24){
		ret = -2;
		goto EXIT;
	}
	
	sceIoLseek(fd, header.offset, PSP_SEEK_SET);
	
	buf = mem_alloc(READ_BMP_LINE_SIZE);
	if(buf == NULL) {
		ret = -3;
		goto EXIT;
	}
	
	vram_p += 512*271;
	for(h=271; h>=0; h -= 8){
		ret = sceIoRead(fd, buf, READ_BMP_LINE_SIZE);
		if( ret != READ_BMP_LINE_SIZE ){
			ret = -4;
			goto EXIT;
		}
		for(j=0,p = buf; j<8; j++){
			for(i=0; i<480; i++, p+=3){
				vram_p[i] = (0x8000 | ((p[0] << 7) & 0x7C00)) | ((p[1] << 2) & 0x3E0) | (p[2] >> 3);
			}
			vram_p -= 512;
		}
	}
	sceIoClose(fd);
	
EXIT:
	strcpy(strrchr(path, '/'), "/bg.tmp");
	if(ret >= 0){
		fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
		sceIoWrite(fd, vram, VRAM_SIZE);
	}
	if( !(fd < 0) ){
		sceIoClose(fd);
	}
	if( buf != NULL ){
		mem_free(buf);
	}
	return ret;
}

//load and draw with 5551 format
int draw_image(const char *path, u16 *vram)
{
	SceUID fd = -1;
	int ret = 0;
	
	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
	if( fd < 0 ){
		ret = -1;
		goto EXIT;
	}
	ret = sceIoRead(fd, vram, VRAM_SIZE);
	if( ret != VRAM_SIZE ){
		ret = -2;
		goto EXIT;
	}
	
EXIT:
	if( !(fd < 0) ){
		sceIoClose(fd);
	}
	return ret;
}

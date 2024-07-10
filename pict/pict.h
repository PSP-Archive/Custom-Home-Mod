#ifndef __PICT_H__
#define __PICT_H__

#include "common.h"
#include "bitmap.h"


#define SCREEN_SHOT_BUFFER_SIZE (SCREEN_SHOT_WIDTH*SCREEN_SHOT_HEIGHT*2)
#define SCREEN_SHOT_WIDTH (480 / 2)
#define SCREEN_SHOT_HEIGHT (272 / 2)
#define BMP_SIZE 391734

typedef struct PictData_{
	void *raw;
	u32 size;
	u32 width;
	u32 height;
}PictData;


int loadBmp(const char *path, PictData *data);
int takeScreenShot(const char *path);
void drawPict5551(int x, int y, PictData *data, libm_draw_info *dinfo);

inline u32 libmConvert5551_4444( u32 src );
inline u32 libmConvert5551_5650( u32 src );


// macro
#define ChangeFormatToBmpFormat(path) \
	char *end = strchr(path, '\0');\
	end[-3] = 'b';\
	end[-2] = 'm';\
	end[-1] = 'p';\

#define ChangeFormatToBinFormat(path) \
	end = strchr(path, '\0');\
	end[-3] = 'b';\
	end[-2] = 'i';\
	end[-1] = 'n';\


#endif

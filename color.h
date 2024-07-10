#ifndef PB_COLOR_H
#define PB_COLOR_H

#define	SetAlpha(color,alpha)	((color & 0xffffff) | ((alpha & 0xff) << 24))
#define	GetAlpha(color)			((color & 0xff000000) >> 24)


#define	BLACK			0xff000000          //  黒色
#define	WHITE			0xffFFFFFF          //  白色

#define	RED				0xff0000FF          //  赤色
#define	GREEN			0xff00FF00          //  緑色
#define	BLUE			0xffFF0000          //  青色

#define	DARKBLUE		0xff800000          //  紺色
#define	LIGHTBLUE		0xffFA0000          //  水色
#define	BROWN			0xff404080          //  茶色
#define	PINK			0xffFF80FF          //  桃色
#define	DARKGREEN		0xff008000          //  深緑色
#define	YELLOGREEN		0xff00FF80          //  黄緑色
#define	PURPLE			0xffFF0080          //  紫色
#define	ORANGE			0xff4080FF          //  橙色
#define	LEMON			0xff80FFFF          //  レモン色
#define	YELLOW			0xff00FFFF          //  黄色
#define	SKYBLUE			0xffFFFF00          //  空色
#define	PEARLORANGE		0xff75BAFF          //  肌色
#define	GRAY			0xff808080          //  灰色
#define	SILVER			0xffC0C0C0          //  銀色
#define	GOLD			0xff20C0E6          //  金色


#define	Invalid			LIBM_NO_DRAW


//    32ビットカラーの16進数コードを ＲＧＢから作成する計算式
//#define  RGB(r,g,b) (((long)b<<16)|((long)g<<8)|((long)r)



#endif

#ifndef COMMON_H
#define COMMON_H

#include <pspctrl.h>
#include <pspctrl_kernel.h>
#include <pspdisplay.h>
#include <pspdisplay_kernel.h>
#include <pspimpose_driver.h>
#include <pspkernel.h>
#include <pspmoduleinfo.h>
#include <psppower.h>
#include <psprtc.h>
#include <pspsdk.h>
#include <pspsysevent.h>
#include <kubridge.h>
#include <systemctrl.h>
#include <systemctrl_se.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "cmlib/cmlibaudio.h"
#include "cmlib/cmlibctrl.h"
#include "cmlib/cmlibmenu.h"
#include "syslibc/sysclib.h"
#include "struct.h"
#include "cushome.h"
#include "pict/pict.h"
#include "memory.h"
#include "libinip.h"
#include "color.h"
#include "states.h"
#include "getname.h"
#include "menu.h"
#include "thread.h"

//マクロ--------------------------------------------------------

//#define MAKE_CALL(a, f)	_sw(0x0C000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a);
#define MAKE_JUMP(a, f) _sw(0x08000000 | (((u32)(f) & 0x0ffffffc) >> 2), a); 
//#define PSP_FIRMWARE(f) ((((f >> 8) & 0xF) << 24) | (((f >> 4) & 0xF) << 16) | ((f & 0xF) << 8) | 0x10)

#define EBOOTBIN "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN"
#define UMDSFO "disc0:/PSP_GAME/PARAM.SFO"


libm_draw_info dinfo;
libm_vram_info vinfo;

libm_draw_info bar_d[4];
libm_vram_info bar_v[4];

libm_draw_info menu_d;
libm_vram_info menu_v;
libm_draw_info check_d;
libm_vram_info check_v;

extern String string;
extern CONFIG conf;
extern bool setting_flag, states_flag, menu_flag;
extern bool thread;
extern int model;
extern char prxPath[];
extern char imagePath[];
extern char drive[];
extern bool check_flag;
extern int setting_pos, menu_pos;
extern char dialog_str[];
extern int usb;
extern int right_pos[];

extern char gamename[];
extern char *gameid;

extern bool StopGame;
extern bool maskflag, muteflag;
extern bool sub_flag;
extern bool suspendCB;
extern bool once_print[];
extern char *ColorList[];
extern int backKey, selectKey;
extern u32 draw_p;
extern u32 color[];

//スレッド停止
#define	LIBM_TSUSPEND				LIBM_TCMD_SUSPEND
#define	LIBM_TRESUME				LIBM_TCMD_RESUME

#define MAX_NUMBER_OF_THREADS 		64
#define	PSP_SYSEVENT_SUSPEND_QUERY	0x100
#define	ITEM_COUNT					80

u16 *tempar;
#define home_col 0x9ce7

typedef enum {
	Color_none,
	Color_main_b,
	Color_main_f,
	Color_nom_b1,
	Color_nom_b2,
	Color_nom_f1,
	Color_nom_f2,
	Color_act_b1,
	Color_act_f1,
	Color_bright,
	Color_bat_gre,
	Color_bat_yel,
	Color_bat_ora,
	Color_bat_red
} Color_num;



//*********************************************************
// 文字列 String の文字列 From を文字列 To で置換する。
// 置換後の文字列 String のサイズが String の記憶領域を超える場合の動作は未定義。
//*********************************************************
char *StrReplace( char *String, const char *From, const char *To );
void Redraw();
int mySuspendThread();
int myResumeThread();

int loadLibraries( void );

int GetColerNum(int color);
void SetColorName(int num, int colornum);
int sceKernelCheckExitCallback(void);
int LoadStartModule(char *module);
int connect_usb();
int DialogMethod();
void DrowMethod();
void CloseMethod();

char *GetMenuStr(int num);
u32 GetHomeAddr();


void PspLsLibrarySaveStates(char number, int flag);
void PspLsLibraryLoadStates(char number, int flag);

void ClearCaches(void);
int Callback_Suspend(int ev_id, char *ev_name, void *param, int *result);

void GetButtons(SceCtrlData *pad, int fwait, int nwait, int (*func)(bool check, int button, void *ptr_), void *ptr);
void write_conf(CONFIG *conf_p);
void make_conf(CONFIG *conf_p);

//LoadExecVSH
int LoadExecVSH(int apitype, char *path);

void qsort(void *base, unsigned num, unsigned width, int (*comp)(const void *, const void *));

void *getmem_vram(int size);
int free_vram(void *ptr);


int draw_image(const char *path, u16 *vram);
int init_image(const char *path, u16 *vram);


#endif
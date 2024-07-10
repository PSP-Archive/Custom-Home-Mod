#include "common.h"
#include "cushome.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  ClearCaches
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void ClearCaches(void)
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  INIT METHOD	
//  Thanks SnyFbSx , estuibal , hiroi01
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define NELEMS(a) (sizeof(a) / sizeof(a[0]))

typedef struct _tag_prxliblist {
    char *mod_name;
    char *prx_path;
} prxLibList;

int LoadStartModule(char *module)
{
	SceUID mod = sceKernelLoadModule(module, 0, NULL);

	if (mod < 0) return mod;

	return sceKernelStartModule(mod, strlen(module)+1, module, NULL, NULL);
}

int loadLibraries( void )
{
	int i;
	SceIoStat stat;
	prxLibList pll[] = {
		{ "cmlibCtrl",  "ms0:/seplugins/lib/cmlibctrl.prx"  },
		{ "cmlibAudio", "ms0:/seplugins/lib/cmlibaudio.prx" },
		{ "cmlibMenu",  "ms0:/seplugins/lib/cmlibmenu.prx"  },
		{ "cmlibConv",  "ms0:/seplugins/lib/cmlibconv.prx"  }
	};
	
	while( sceKernelFindModuleByName( "sceKernelLibrary" ) == NULL ) sceKernelDelayThread( 1000 );

	for( i = 0; i < NELEMS(pll); i++ )
	{
		if( sceKernelFindModuleByName(pll[i].mod_name) == NULL )
		{
			if(sceIoGetstat(pll[i].prx_path, &stat) < 0)
			{
				pll[i].prx_path[0] = 'e'; pll[i].prx_path[1] = 'f';
				if(sceIoGetstat(pll[i].prx_path, &stat) < 0)continue;
			}
			LoadStartModule(pll[i].prx_path);
		}
	}
		
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  Redraw
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Redraw()
{
	int i;
	u16 *vram = dinfo.vinfo->buffer;
	if(draw_image(imagePath, vram) < 0){
		for(i=0; i<512*272; i++)vram[i] = home_col;
	}
	initPrintStatusBar(&conf, &dinfo);
	PrintMainMenu(&conf, &dinfo);
}

int mySuspendThread()
{
	/*
	if(thread)return -1;
	suspendThreads();
	thread = Get_ThreadModeStatus();
	return 0;
	*/
	return threadCtrlSuspend();	
}

int myResumeThread()
{
	/*
	if(!thread)return -1;
	resumeThreads();
	thread = Get_ThreadModeStatus();
	return 0;
	*/
	return threadCtrlResume();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  CloseMethod
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void CloseMethod()
{
	menu_pos = 0;
	if(usb)usb = connect_usb();
	myResumeThread();
	StopGame = false;
	menu_flag = false;
	sub_flag = false;
	libctrlMaskAllButtonOff(&maskflag);
	libaudioIoEnable(&muteflag);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  LoadExecVSH
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int LoadExecVSH(int apitype, char *path)
{
	struct SceKernelLoadExecVSHParam param;

	memset(&param, 0, sizeof(param));
	param.size = sizeof(param);

	switch(apitype)
	{
		case 0x141: //homebrew mode
		case 0x152: //homebrew PSPGo
			param.args = strlen(path)+1;
			param.argp = path;
			param.key = "game";
			break;
		case 0x120: //ISO mode
		case 0x123:
		case 0x125: //ISO PSPGo
			param.args = strlen(EBOOTBIN)+1;
			param.argp = EBOOTBIN;
			param.key = (apitype == 0x120 ? "game" : "umdemu");
			break;
		case 0x124://PBOOT.PBP(これで動くかわからない)
		case 0x126://Go
			param.args = strlen(path)+1;
			param.argp = path;
			param.key = "game";
			break;
/*		case 0x144: // POPS mode
		case 0x155: // POPS PSPGo
			param.args = strlen(path)+1;
			param.argp = path;
			param.key = "pops";
			break;
*/	}
	sctrlKernelLoadExecVSHWithApitype( apitype , path , &param);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  StrReplace
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//http://katsura-kotonoha.sakura.ne.jp/prog/c/tip0000e.shtml
//部分文字列を置換する　　より引用
//*********************************************************
// 文字列 String を nShift だけ移動する。
// 移動先へのポインタを返す。
//*********************************************************
char *StrShift( char *String, size_t nShift )
{
	char *start = String;
	char *stop  = String + strlen( String );
	memmove( start + nShift, start, stop-start+1 );

	return String + nShift;
}//StrShift

//*********************************************************
// 文字列 String の文字列 From を文字列 To で置換する。
// 置換後の文字列 String のサイズが String の記憶領域を超える場合の動作は未定義。
//*********************************************************
char *StrReplace( char *String, const char *From, const char *To )
{
	int   nToLen;   // 置換する文字列の長さ
	int   nFromLen; // 検索する文字列の長さ
	int   nShift;
	char *start;    // 検索を開始する位置
	char *stop;     // 文字列 String の終端
	char *p;

	nToLen   = strlen( To );
	nFromLen = strlen( From );
	nShift   = nToLen - nFromLen;
	start    = String;
	stop     = String + strlen( String );

	// 文字列 String の先頭から文字列 From を検索
	while( NULL != ( p = strstr( start, From ) ) )
	{
		// 文字列 To が複写できるようにする
		start = StrShift( p + nFromLen, nShift );
		stop  = stop + nShift;

		// 文字列 To を複写
		memmove( p, To, nToLen );
	}

	return String;
}//StrReplace


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  cusHomeGetStatus
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int cusHomeGetStatus()
{
	return (menu_flag ? 1 : 0);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  cusHomeClose
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int cusHomeClose()
{
	if(menu_flag)
	{
		if(usb)usb = connect_usb();
		myResumeThread();
		StopGame = false;
		menu_flag = false;
		sub_flag = false;
		libctrlMaskAllButtonOff(&maskflag);
		libaudioIoEnable(&muteflag);
		setting_flag = false;
		states_flag = false;
		check_flag = false;
		sceDisplaySetFrameBufferInternal(1, 0, 512, PSP_DISPLAY_PIXEL_FORMAT_5551, 1);
		return 0;
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  Callback_Suspend
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int Callback_Suspend(int ev_id, char *ev_name, void *param, int *result)
{
	// 返り値に 0 ではなく -1 を返すと、スリープを無効化できる
	
	// スリープ時
	if( ev_id == PSP_SYSEVENT_SUSPEND_QUERY)
	{
		cusHomeClose();
	}
	else if(ev_id == 0x400000)	// Resume Complete
	{
		suspendCB = true;
	}

	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  GetColerNum
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int GetColerNum(int color)
{
	switch(color){
		case BLACK:			return 1;
		case BLUE:			return 2;
		case RED:			return 3;
		case GREEN:			return 4;
		case DARKBLUE:		return 5;
		case LIGHTBLUE:		return 6;
		case BROWN:			return 7;
		case PINK:			return 8;
		case DARKGREEN:		return 9;
		case YELLOGREEN:	return 10;
		case PURPLE:		return 11;
		case ORANGE:		return 12;
		case LEMON:			return 13;
		case YELLOW:		return 14;
		case SKYBLUE:		return 15;
		case PEARLORANGE:	return 16;
		case GRAY:			return 17;
		case SILVER:		return 18;
		case GOLD:			return 19;
		case WHITE:			return 20;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  GetButtons
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool FirstFlag;
bool SecondFlag;
clock_t time_count;

void GetButtons(SceCtrlData *pad, int fwait, int nwait, int (*func)(bool check, int button, void *ptr_), void *ptr)
{
	bool flag = false;
	int wait[2] = {fwait, nwait};
	
	if(func(true, pad->Buttons, ptr) > 0)
	{
		if(FirstFlag)
		{
			if(sceKernelLibcClock() - time_count > wait[SecondFlag])
			{
				SecondFlag = true;
				flag = true;
			}
		} else {
			FirstFlag = true;
			flag = true;
		}
	} else {
		FirstFlag = SecondFlag = false;
	}
	
	if(flag)
	{
		time_count = sceKernelLibcClock();
		func(false, pad->Buttons, ptr);
	}
	return;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  GetButtons
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

u32 GetHomeAddr()
{
	u32 text_addr, text_end, addr=0;
	SceModule2 *mod = sceKernelFindModuleByName("sceImpose_Driver");
	if(mod == NULL)return -1;
	
	text_addr = mod->text_addr;
	text_end = mod->text_addr + mod->text_size;
	
	for(; text_addr < text_end; text_addr += 4)
	{
		if(_lw(text_addr) == 0x24060200 && _lw(text_addr + 8) == 0x24070001)
		{
			u32 temp1 = (_lw(text_addr - 8) & 0xffff);
			u32 temp2 = (mod->text_addr & 0xffff0000) + temp1;
			if((mod->text_addr & 0xffff) >= temp1)
			{
				temp2 += 0x10000;
			}
			addr = _lw(_lw(temp2) + (_lw(text_addr + 4) & 0xffff));
			break;
		}
	}
	return addr;
}

/*
static u32 head = 0x04200000 - 4;

void *getmem_vram(int size)
{
	*(u32 *)(head -size - 4) = head;
	head -= size - 4;
	return (void *)head+4;
}

int free_vram(void *ptr)
{
	int size = (u32)ptr-4 - head;
	u32 move_addr = *(u32 *)(ptr - 4) - size;
	memmove((void *)move_addr, (void *)head, size);
	memset((void *)head, 0, move_addr-head);
	head = move_addr;
	if(size != 0)
		*(u32 *)head 
		= move_addr 
		+ size;
	return 0;
}
*/

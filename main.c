/*
CustomHOME

協力してくださったhiroi01氏、SnyFbSx氏、j416氏、mafu氏、plum氏
ありがとうございました。

このプラグインを作成するにあたって、
	iniLibrary			by hiroi01氏
	LibMenu				by maxem氏
上記のライブラリーを使用させてもらいました。
ありがとうございます。

*/


#include "common.h"

PSP_MODULE_INFO("CustomHOME", PSP_MODULE_KERNEL, 0, 9);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  変数
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define LANG_KEY_NUM 35
#define LANG_INI "/lang.ini"
#define LANG_BIN "/cushome.lang"

#define CONF_KEY_NUM 31
#define CONF_INI "/conf.ini"
#define CONF_BIN "/cushome.conf"

ILP_Key lang_key[LANG_KEY_NUM];
ILP_Key conf_key[CONF_KEY_NUM];

char item[9][128];
char menu[8][128];
char conf_ini[128];
char conf_bin[128];
int bartype;
bool menu_print_pos;

String string;

char iniPath[128];
char prxPath[128];
char imagePath[128];
char drive[8];
char gamename[128];
char *gamepath;
char *gameid;

int menu_pos;
int setting_pos;
int backKey, selectKey;
int usb = 0;
int model;

bool StopGame = false;
bool states_flag;
bool muteflag = false;
bool maskflag = false;
bool setting_flag;
bool menu_flag = false;
bool check_flag = false;

bool thread = false;
bool suspendCB = false;

u32 colorCode[21] = 
{
	0			,
	BLACK		,
	BLUE		,
	RED			,
	GREEN		,
	DARKBLUE	,
	LIGHTBLUE	,
	BROWN		,
	PINK		,
	DARKGREEN	,
	YELLOGREEN	,
	PURPLE		,
	ORANGE		,
	LEMON		,
	YELLOW		,
	SKYBLUE		,
	PEARLORANGE	,
	GRAY		,
	SILVER		,
	GOLD		,
	WHITE		,
};

#define MAXCPULIST	9
const int cpu_list[]={0, 20, 75, 100, 133, 166, 222, 266, 300, 333};
const int bus_list[]={0, 10, 37, 50, 66, 83, 111, 133, 150, 166};
int currclockspeed  = 6;

int sceKernelInvokeExitCallback();

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  logWrite		Thanks takka
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
char debugBuffer[256];
// debug log start ***********************
//sprintf(debugBuffer, "%s", "call loadIcon0.");
//logWrite(debugBuffer);
// debug log end   ***********************
//-----------------------------------------
void logWrite(char *str)
{
	//int i;
	char msg[256];

	SceUID fp;
	fp = sceIoOpen("ms0:/log.txt", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND, 0777);

	if(str)
	{
		sprintf(msg, "%s\n", str);
		sceIoWrite(fp, msg, strlen(msg));
	}

	sceIoClose(fp);
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  dumy_func
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int dumy_func(int value)
{
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  DialogMethod
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern int d_width;

int DialogMethod()
{
	SceCtrlData pad;
	int ret = 0;
	
	initPrintDialog(&dinfo);
	PrintDialog(&dinfo);
	
	while(1){
		sceKernelDelayThread(1000);
		sceCtrlPeekBufferPositive(&pad, 1);
		if(pad.Buttons & (PSP_CTRL_RIGHT|PSP_CTRL_LEFT))
		{
			check_flag ^= true;
			PrintDialog(&dinfo);
			libctrlWaitButtonUp(PSP_CTRL_RIGHT | PSP_CTRL_LEFT);
		}
		else if((pad.Buttons & selectKey) && check_flag)
		{
			libctrlWaitButtonUp(selectKey);
			ret = 1;
			break;
		}
		else if(((pad.Buttons & selectKey)&&( check_flag == false))||(pad.Buttons & backKey))
		{
			libctrlWaitButtonUp(selectKey | backKey);
			ret = 0;
			break;
		}
		else if(pad.Buttons & PSP_CTRL_HOME)
		{
			libctrlWaitButtonUp(PSP_CTRL_HOME);
			ret = -1;
			break;
		}
	}
	check_flag =false;
	d_width = 0;

	return ret;
}




int sub_pos = 0;
bool sub_flag = false;

typedef struct Color_ {
	u32 *ptr[8];
	int num[8];
	int orig[8];
	int buck[8];
	CONFIG *conf_p;
} Color;

char SettingLang[16][64];

const char *LangOnOff[2] =
	{
		"[OFF]",
		"[O N]"
	};

const char *LangUpDown[2] =
	{
		"[DOWN]",
		"[ UP ]"
	};

void cpMainSettingStr(CONFIG *conf_p)
{
	sprintf(SettingLang[0], "%sSwapButton", LangOnOff[conf_p->swap]);
	sprintf(SettingLang[1], "%sMute", LangOnOff[conf_p->mute]);
	sprintf(SettingLang[2], "%sStopGame", LangOnOff[conf_p->stop]);
	sprintf(SettingLang[3], "%sMenuPos", LangUpDown[menu_print_pos]);
	strcpy(SettingLang[4], "ColorSetting");
	strcpy(SettingLang[5], "MenuSetting");
	strcpy(SettingLang[6], "BarSetting");
}

void cpMenuSettingStr(CONFIG *conf_p)
{
	int i;
	for(i=0; i<8; i++)
	{
		sprintf(SettingLang[i], "%s%s", LangOnOff[conf_p->menu[i].view], GetMenuStr(conf_p->menu[i].true_num));
	}
	strcpy(SettingLang[8], "\0");
	sprintf(SettingLang[9], "Switch : %s",(conf_p->swap ? "○" : "×"));
	strcpy(SettingLang[10], "Sort : □ & ↑↓");
}


char *ColorLang[8] = {
	"MainBackColor",
	"MainFontColor",
	"NormalBackColor1",
	"NormalBackColor2",
	"NormalFontColor1",
	"NormalFontColor2",
	"ActiveBackColor1",
	"ActiveFontColor2"
};

void cpColorSettingStr(CONFIG *conf_p)
{
	sprintf(SettingLang[8], "%s%s", LangOnOff[conf_p->color.twin], "DoubleColor");
	strcpy(SettingLang[9], "\0");
	strcpy(SettingLang[10], "Select : ←→");
	sprintf(SettingLang[11], "Decision/Switch : %s",(conf_p->swap ? "○" : "×"));
}


char *ItemLang[] = {
	"CPU/BUS",
	"Description",
	"Brightness",
	"Gameid",
	"Gamename",
	"Volume",
	"VolGauge",
	"Date",
	"Battery",
};

char right_icon[] = { 0xFF, 0x00, 0x1F, 0x00, 0xFF, 0x00, 0x1F, 0x00 };
char left_icon[] = { 0xFF, 0x00, 0xF8, 0x00, 0xFF, 0x00, 0xF8, 0x00 };


void SetColorName(int num, int colornum)
{
	sprintf(SettingLang[num], "%s:← %s →", ColorLang[num], ColorList[colornum]);
}

void cpBarSettingStr(CONFIG *conf_p)
{
	int i;
	char *ptr;
	for(i=0; i<9; i++)
	{
		if(conf_p->item[i].view == false)
		{
			ptr = (char *)LangOnOff[0];
		} else {
			ptr = "[   ]";
		}
		sprintf(SettingLang[i], "%s%s", ptr, ItemLang[conf_p->item[i].true_num]);
	}
	sprintf(SettingLang[9], "BarPattern: %d", bartype);
	strcpy(SettingLang[10], "\0");
	strcpy(SettingLang[11], "Priority : □ & ↑↓");
	strcpy(SettingLang[12], "Position : △&↑↓←→ +　R:Speed Up");
	sprintf(SettingLang[13], "Justification : %s",(conf_p->swap ? "○" : "×"));
	strcpy(SettingLang[14], "Bar Pattern : ←→");
	
}


void PrintMainSetting(int num, int pos, CONFIG *conf_p, libm_draw_info *dinfo)
{
	int i, count, f_color, b_color;
	int x=120, y = 90;
	for(i=0, count=0; i< num; i++, y+=9)
	{
		f_color = (pos == i ? color[Color_act_f1] : color[Color_nom_f1]);
		b_color = (pos == i ? color[Color_act_b1] : ( conf_p->color.twin && count%2 ? color[Color_nom_b1] : color[Color_nom_b2]) );
		libmFillRect(x, y, x+30*8, y+9, b_color, dinfo);
		count++;
		libmPrintXY(x, y+1, f_color, 0, SettingLang[i], dinfo);
	}
}

void PrintBarSetting(int pos, CONFIG *conf_p, libm_draw_info *dinfo)
{
	int i, count, f_color;
	int x=120, y = 90;
	char *icon;
	for(i=0, count=0; i<9; i++, y+=9)
	{
		if(conf_p->item[i].view)
		{
			f_color = (pos == i ? color[Color_act_f1] : color[Color_nom_f1]);
			count++;
			icon = (conf_p->item[i].right ? right_icon : left_icon);
			libmPrintMyIcon(x+8*2, y+1, f_color, 0, icon, 8, dinfo);
		}
	}
}


int SetColorButton(bool check, int button, void *ptr)
{
	Color *colorInfo = ptr;
	
	if(button & (PSP_CTRL_UP|PSP_CTRL_DOWN))
	{
		if(check)return 1;
		//UP -> -1, DOWN -> +1
		sub_pos += (((int)((button&(PSP_CTRL_UP|PSP_CTRL_DOWN))-40))/24);
		if(sub_pos < 0) sub_pos = 8;
		if(sub_pos > 8) sub_pos = 0;
		PrintMainSetting(9, sub_pos, colorInfo->conf_p, &dinfo);
		return 0;
	}
	
	if(sub_pos != 8 && button&(PSP_CTRL_RIGHT|PSP_CTRL_LEFT))
	{
		if(check)return 1;
		//LEFT -> -1, RIGHT -> +1
		colorInfo->num[sub_pos] += (((int)((button&(PSP_CTRL_RIGHT|PSP_CTRL_LEFT))-80))/-48);
		if(colorInfo->num[sub_pos] > 20)colorInfo->num[sub_pos] = (colorInfo->orig[sub_pos] ? 0 : 1);
		if(colorInfo->num[sub_pos] < (colorInfo->orig[sub_pos] ? 0 : 1))colorInfo->num[sub_pos] = 20;
		*colorInfo->ptr[sub_pos] = (colorInfo->num[sub_pos] ? 
				colorCode[colorInfo->num[sub_pos]] : colorInfo->orig[sub_pos]);
		SetColorName(sub_pos, colorInfo->num[sub_pos]);
		setColor();
		PrintMainMenu(&conf, &dinfo);
		PrintMainSetting(9, sub_pos, colorInfo->conf_p, &dinfo);
		return 0;
	}
	
	else if(button & selectKey)
	{
		if(check)return 1;
		libctrlWaitButtonUp(selectKey);
		if(sub_pos == 8)
		{
			colorInfo->conf_p->color.twin ^= true;
			cpColorSettingStr(colorInfo->conf_p);
		} else {
			colorInfo->buck[sub_pos] = *colorInfo->ptr[sub_pos];
			if(colorInfo->num[sub_pos] != 0)colorInfo->orig[sub_pos] = 0;
		}
		PrintMainSetting(9, sub_pos, colorInfo->conf_p, &dinfo);
		return 0;
	}
	else if(button & (PSP_CTRL_SELECT|backKey|PSP_CTRL_HOME))
	{
		if(check)return 1;
		libctrlWaitButtonUp(PSP_CTRL_SELECT|backKey|PSP_CTRL_HOME);
		if(button & PSP_CTRL_HOME)cusHomeClose();
		sub_flag = false;
		Redraw();
		return 0;
	}
	return -1;
}


typedef struct MInfo_ {
	int true_num;
	int menu_num;
	bool view;
} MInfo;

typedef struct ItemInfo_{
	int true_num;
	int menu_num;
	int bar_num;
	int x_addr;
	bool right;
	bool view;
} ItemInfo;


/*
//Thanks http://simd.jugem.jp/?eid=116
*/

int item_true_comp( const void *c1, const void *c2 )
{
  ItemInfo *info1 = (ItemInfo *)c1;
  ItemInfo *info2 = (ItemInfo *)c2;

  int tmp1 = info1->true_num; 
  int tmp2 = info2->true_num;

  return tmp1 - tmp2;
}

int item_menu_comp( const void *c1, const void *c2 )
{
  ItemInfo *info1 = (ItemInfo *)c1;
  ItemInfo *info2 = (ItemInfo *)c2;

  int tmp1 = info1->menu_num; 
  int tmp2 = info2->menu_num;

  return tmp1 - tmp2;
}

void item_ItemSwap(ItemInfo *data1, ItemInfo *data2)
{
	int temp;
	temp = data1->true_num;
	data1->true_num = data2->true_num;
	data2->true_num = temp;
	
	temp = data1->bar_num;
	data1->bar_num = data2->bar_num;
	data2->bar_num = temp;
	
	temp = data1->x_addr;
	data1->x_addr = data2->x_addr;
	data2->x_addr = temp;
	
	if(data1->view != data2->view){
		data1->view ^= true;
		data2->view ^= true;
	}
	if(data1->right != data2->right){
		data1->right ^= true;
		data2->right ^= true;
	}
}


int menu_true_comp( const void *c1, const void *c2 )
{
  MInfo *info1 = (MInfo *)c1;
  MInfo *info2 = (MInfo *)c2;

  int tmp1 = info1->true_num; 
  int tmp2 = info2->true_num;

  return tmp1 - tmp2;
}

int menu_menu_comp( const void *c1, const void *c2 )
{
  MInfo *info1 = (MInfo *)c1;
  MInfo *info2 = (MInfo *)c2;

  int tmp1 = info1->menu_num; 
  int tmp2 = info2->menu_num;

  return tmp1 - tmp2;
}


void menu_ItemSwap(MInfo *data1, MInfo *data2)
{
	int temp = data1->true_num;
	data1->true_num = data2->true_num;
	data2->true_num = temp;
	
	if(data1->view != data2->view){
		data1->view ^= true;
		data2->view ^= true;
	}
}

int SetMenuButton(bool check, int button, void *ptr)
{
	CONFIG *conf_p = ptr;
	
	if(button & PSP_CTRL_SQUARE)
	{
		if(button & (PSP_CTRL_UP|PSP_CTRL_DOWN))
		{
			if(check)return 1;
			//UP -> -1, DOWN -> +1
			int target = sub_pos;
			int i;
			sub_pos += (((int)((button&(PSP_CTRL_UP|PSP_CTRL_DOWN))-40))/24);
			if(sub_pos < 0)
			{
				sub_pos = 7;
				for(i=0; i<7; i++)menu_ItemSwap((MInfo *)&conf_p->menu[i], (MInfo *)&conf_p->menu[i+1]);
			}
			else if(sub_pos > 7)
			{
				sub_pos = 0;
				for(i=7; i>0; i--)menu_ItemSwap((MInfo *)&conf_p->menu[i], (MInfo *)&conf_p->menu[i-1]);
			} else {
				menu_ItemSwap((MInfo *)&conf_p->menu[sub_pos], (MInfo *)&conf_p->menu[target]);
			}
			cpMenuSettingStr(conf_p);
			PrintMainSetting(8, sub_pos, conf_p, &dinfo);
			return 0;
		}
	}
	else if(button & (PSP_CTRL_UP|PSP_CTRL_DOWN))
	{
		if(check)return 1;
		//UP -> -1, DOWN -> +1
		sub_pos += (((int)((button&(PSP_CTRL_UP|PSP_CTRL_DOWN))-40))/24);
		if(sub_pos < 0) sub_pos = 7;
		if(sub_pos > 7) sub_pos = 0;
		PrintMainSetting(8, sub_pos, conf_p, &dinfo);
		return 0;
	}
	
	else if(button & selectKey)
	{
		if(check)return 1;
		libctrlWaitButtonUp(selectKey);
		conf_p->menu[sub_pos].view ^= true;
		if(model == 0 && conf_p->menu[sub_pos].true_num == 7)
		{
			conf_p->menu[sub_pos].view = false;
			return 0;
		}
		if(conf_p->menu[sub_pos].true_num == 4 && conf_p->menu[sub_pos].view)
		{
			conf_p->stop = false;
		}
		int i;
		for(i=0, conf_p->menu_itm=0; i<8; i++)
		{
			if(conf_p->menu[i].view)conf_p->menu_itm++;
		}
		cpMenuSettingStr(conf_p);
		PrintMainSetting(8, sub_pos, conf_p, &dinfo);
		return 0;
	}
	else if(button & (PSP_CTRL_SELECT|backKey|PSP_CTRL_HOME))
	{
		if(check)return 1;
		libctrlWaitButtonUp(PSP_CTRL_SELECT|backKey|PSP_CTRL_HOME);
		if(button & PSP_CTRL_HOME)cusHomeClose();
		sub_flag = false;
		Redraw();
		return 0;
	}
	return -1;
}


bool bar_ptrn[8][4] = {
	{true, false, false, false},
	{true, true, false, false},
	{false, false, false, true},
	{false, false, true, true},
	{true, false, false, true},
	{true, true, false, true},
	{true, false, true, true},
	{true, true, true, true},
};

void getBarType(CONFIG *conf_p)
{
	int i, j;
	for(j=0; j<8; j++)
	{
		for(i=0; i<4; i++)
		{
			if(conf_p->bar[i] != bar_ptrn[j][i])break;
			if(i == 3)
			{
				bartype =j;
				return;
			}
		}
	}
}

int bar_count[9];

int SetBarButton(bool check, int button, void *ptr)
{
	CONFIG *conf_p = ptr;
	
	if(sub_pos != 9 && button & PSP_CTRL_TRIANGLE)
	{
		if(button & (PSP_CTRL_UP|PSP_CTRL_DOWN))
		{
			if(check)return 1;
			//UP -> -1, DOWN -> +1
			int updown = (((int)((button&(PSP_CTRL_UP|PSP_CTRL_DOWN))-40))/24);
			while(1)
			{
				conf_p->item[sub_pos].bar_num += updown;
				if(conf_p->item[sub_pos].bar_num < 0) conf_p->item[sub_pos].bar_num = 3;
				if(conf_p->item[sub_pos].bar_num > 3) conf_p->item[sub_pos].bar_num = 0;
				if(conf_p->bar[conf_p->item[sub_pos].bar_num])break;
			}
			initPrintStatusBar(&conf, &dinfo);
			PrintMainMenu(&conf, &dinfo);
			PrintMainSetting(15, sub_pos, conf_p, &dinfo);
			PrintBarSetting(sub_pos, conf_p, &dinfo);
			return 0;
		}
		else if(button&(PSP_CTRL_RIGHT|PSP_CTRL_LEFT))
		{
			if(check)return 1;
			//LEFT -> -1, RIGHT -> +1
			int move_num = (((int)((button&(PSP_CTRL_RIGHT|PSP_CTRL_LEFT))-80))/-48);
			if(button&(PSP_CTRL_RTRIGGER))move_num *= 4;
			conf_p->item[sub_pos].x_addr += move_num;
			if(conf_p->item[sub_pos].right)
			{
				if(conf_p->item[sub_pos].x_addr  > 480)conf_p->item[sub_pos].x_addr -= move_num;
				if(conf_p->item[sub_pos].x_addr - right_pos[conf_p->item[sub_pos].true_num] < 0)conf_p->item[sub_pos].x_addr -= move_num;
			} else {
				if(conf_p->item[sub_pos].x_addr + right_pos[conf_p->item[sub_pos].true_num] > 480)conf_p->item[sub_pos].x_addr -= move_num;
				if(conf_p->item[sub_pos].x_addr < 0)conf_p->item[sub_pos].x_addr -= move_num;
			}
			initPrintStatusBar(&conf, &dinfo);
			PrintMainMenu(&conf, &dinfo);
			PrintMainSetting(15, sub_pos, conf_p, &dinfo);
			PrintBarSetting(sub_pos, conf_p, &dinfo);
			return 0;
		}
	}
	else if(sub_pos != 9 && button & PSP_CTRL_SQUARE)
	{
		if(button & (PSP_CTRL_UP|PSP_CTRL_DOWN))
		{
			if(check)return 1;
			//UP -> -1, DOWN -> +1
			int target = sub_pos;
			int i;
			sub_pos += (((int)((button&(PSP_CTRL_UP|PSP_CTRL_DOWN))-40))/24);
			if(sub_pos < 0)
			{
				sub_pos = 8;
				for(i=0; i<8; i++)item_ItemSwap((ItemInfo *)&conf_p->item[i], (ItemInfo *)&conf_p->item[i+1]);
			} else if(sub_pos > 8)
			{
				sub_pos = 0;
				for(i=8; i>0; i--)item_ItemSwap((ItemInfo *)&conf_p->item[i], (ItemInfo *)&conf_p->item[i-1]);
			} else {
				item_ItemSwap((ItemInfo *)&conf_p->item[sub_pos], (ItemInfo *)&conf_p->item[target]);
			}
			cpBarSettingStr(conf_p);
			initPrintStatusBar(&conf, &dinfo);
			PrintMainMenu(&conf, &dinfo);
			PrintMainSetting(15, sub_pos, conf_p, &dinfo);
			PrintBarSetting(sub_pos, conf_p, &dinfo);
			return 0;
		}
	}
	else if(button & (PSP_CTRL_UP|PSP_CTRL_DOWN))
	{
		if(check)return 1;
		//UP -> -1, DOWN -> +1
		sub_pos += (((int)((button&(PSP_CTRL_UP|PSP_CTRL_DOWN))-40))/24);
		if(sub_pos < 0) sub_pos = 9;
		if(sub_pos > 9) sub_pos = 0;
		PrintMainSetting(15, sub_pos, conf_p, &dinfo);
		PrintBarSetting(sub_pos, conf_p, &dinfo);
		return 0;
	}
	else if(sub_pos == 9 && button&(PSP_CTRL_RIGHT|PSP_CTRL_LEFT))
	{
		if(check)return 1;
		int i;
		//LEFT -> -1, RIGHT -> +1
		bartype += (((int)((button&(PSP_CTRL_RIGHT|PSP_CTRL_LEFT))-80))/-48);
		if(bartype > 7)bartype = 0;
		if(bartype < 0)bartype = 7;
		for(i=0; i<4; i++)conf_p->bar[i] = bar_ptrn[bartype][i];
		conf_p->menu_pos = (menu_print_pos ? (conf_p->bar[0] + conf_p->bar[1])*10 :
			272-(conf_p->bar[2] + conf_p->bar[3] )*10 - (conf_p->menu_itm*9+4));
		cpBarSettingStr(conf_p);
		initPrintStatusBar(conf_p, &dinfo);
		PrintMainMenu(&conf, &dinfo);
		Redraw();
		PrintMainSetting(15, sub_pos, conf_p, &dinfo);
		PrintBarSetting(sub_pos, conf_p, &dinfo);
		return 0;
	}
	
	else if(sub_pos != 9 && button & selectKey)
	{
		if(check)return 1;
		libctrlWaitButtonUp(selectKey);
		bar_count[sub_pos]++;
		if(bar_count[sub_pos] > 2)bar_count[sub_pos] = 0;
		if(bar_count[sub_pos] == 2)
		{
			conf_p->item[sub_pos].view = false;
		} else {
			conf_p->item[sub_pos].view = true;
			conf_p->item[sub_pos].right = bar_count[sub_pos];
			if(conf_p->item[sub_pos].right)
			{
				conf_p->item[sub_pos].x_addr += right_pos[conf_p->item[sub_pos].true_num];
			} else {
				conf_p->item[sub_pos].x_addr -= right_pos[conf_p->item[sub_pos].true_num];
			}
		}
		cpBarSettingStr(conf_p);
		initPrintStatusBar(conf_p, &dinfo);
		Redraw();
		PrintMainSetting(15, sub_pos, conf_p, &dinfo);
		PrintBarSetting(sub_pos, conf_p, &dinfo);
		return 0;
	}
	else if(button & (PSP_CTRL_SELECT|backKey|PSP_CTRL_HOME))
	{
		if(check)return 1;
		libctrlWaitButtonUp(PSP_CTRL_SELECT|backKey|PSP_CTRL_HOME);
		if(button & PSP_CTRL_HOME)cusHomeClose();
		sub_flag = false;
		Redraw();
		return 0;
	}
	return -1;
}


int SettingButton(bool check, int button, void *ptr)
{
	CONFIG *conf_p = ptr;
	SceCtrlData pad;
	int i;
	
	if(button & (PSP_CTRL_UP|PSP_CTRL_DOWN))
	{
		if(check)return 1;
		//UP -> -1, DOWN -> +1
		setting_pos += (((int)((button&(PSP_CTRL_UP|PSP_CTRL_DOWN))-40))/24);
		if(setting_pos < 0) setting_pos = 6;
		if(setting_pos > 6) setting_pos = 0;
		PrintMainSetting(7, setting_pos, conf_p, &dinfo);
		return 0;
	}
	
	else if(button & selectKey)
	{
		if(check)return 1;
		libctrlWaitButtonUp(selectKey);
		switch (setting_pos)
		{
			case 0://SwapButton
				conf_p->swap ^= true;
				selectKey	= (conf_p->swap ? PSP_CTRL_CIRCLE : PSP_CTRL_CROSS);
				backKey		= (conf_p->swap ? PSP_CTRL_CROSS : PSP_CTRL_CIRCLE);
				break;
			
			case 1://Mute
				conf_p->mute ^= true;
				if(conf_p->mute)libaudioIoDisable(&muteflag);
				else libaudioIoEnable(&muteflag);
				break;
			
			case 2://StopGame
				conf_p->stop ^= true;
				StopGame = false;
				for(i=0; i<8; i++)
				{
					if(conf_p->menu[i].true_num == 4)
					{
						if(conf_p->menu[i].view == true)
						{
							conf_p->menu[i].view = false;
							conf_p->menu_itm--;
						}
						break;
					}
				}
				PrintMainMenu(&conf, &dinfo);
				Redraw();
				break;
			
			case 3://Menu_Print_Pos
				menu_print_pos ^= true;
					conf_p->menu_pos = (menu_print_pos ? (conf_p->bar[0] + conf_p->bar[1])*10 :
						272-(conf_p->bar[2] + conf_p->bar[3] )*10 - (conf_p->menu_itm*9+4));
				PrintMainMenu(&conf, &dinfo);
				Redraw();
				break;
			
			case 4://ChangeColor
				sub_pos = 0;
				sub_flag = true;
				Color colorInfo = {
					{
						&conf_p->color.main_b, &conf_p->color.main_f,
						&conf_p->color.nom_b1, &conf_p->color.nom_b2,
						&conf_p->color.nom_f1, &conf_p->color.nom_f2,
						&conf_p->color.act_b1, &conf_p->color.act_f1
					}
				};
				colorInfo.conf_p = conf_p;
				for(i=0; i<8; i++)
				{
					colorInfo.buck[i] = *colorInfo.ptr[i];
					colorInfo.num[i] = GetColerNum(colorInfo.buck[i]);
					if(colorInfo.num[i] == 0)colorInfo.orig[i] = colorInfo.buck[i];
					SetColorName(i, colorInfo.num[i]);
				}
				cpColorSettingStr(conf_p);
				PrintMainSetting(12, sub_pos, conf_p, &dinfo);
				while(sub_flag)
				{
					sceKernelDelayThread(5000);
					sceCtrlPeekBufferPositive(&pad, 1);
					GetButtons(&pad, 500*1000, 150*1000, SetColorButton, &colorInfo);
				}
				for(i=0; i<4; i++)*colorInfo.ptr[i] = colorInfo.buck[i];
				setColor();
				break;
				
			case 5:
				sub_pos = 0;
				sub_flag = true;
				cpMenuSettingStr(conf_p);
				PrintMainSetting(11, sub_pos, conf_p, &dinfo);
				while(sub_flag)
				{
					sceKernelDelayThread(5000);
					sceCtrlPeekBufferPositive(&pad, 1);
					GetButtons(&pad, 500*1000, 150*1000, SetMenuButton, (void *)conf_p);
				}
				PrintMainMenu(&conf, &dinfo);
				break;
			case 6:
				sub_pos = 0;
				sub_flag = true;
				for(i=0; i<9; i++)
				{
					if(conf_p->item[i].view == false)
					{
						bar_count[i] = 2;
					} else {
						bar_count[i] = conf_p->item[i].right;
					}
				}
				getBarType(conf_p);
				cpBarSettingStr(conf_p);
				PrintMainSetting(15, sub_pos, conf_p, &dinfo);
				PrintBarSetting(sub_pos, conf_p, &dinfo);
				while(sub_flag)
				{
					sceKernelDelayThread(5000);
					sceCtrlPeekBufferPositive(&pad, 1);
					GetButtons(&pad, 500*1000, 150*1000, SetBarButton, (void *)conf_p);
				}
				break;
		}
		cpMainSettingStr(conf_p);
		PrintMainSetting(7, setting_pos, conf_p, &dinfo);
		return 0;
	}
	else if(button & (PSP_CTRL_SELECT|backKey|PSP_CTRL_HOME))
	{
		if(check)return 1;
		libctrlWaitButtonUp(PSP_CTRL_SELECT|backKey|PSP_CTRL_HOME);
		if(button & PSP_CTRL_HOME)cusHomeClose();
		setting_flag = false;
		Redraw();
		return 0;
	}
	return -1;
}

void Data2Str(int type, CONFIG *conf_p, int num, char *str)
{
	switch(type)
	{
		case 0:
			sprintf(str, "%d,%d,%d,%d,%d", conf_p->item[num].menu_num, conf_p->item[num].bar_num, conf_p->item[num].x_addr,
				(int)conf_p->item[num].right, (int)conf_p->item[num].view);
			break;
		
		case 1:
			sprintf(str, "%d,%d", conf_p->menu[num].menu_num, (int)conf_p->menu[num].view);
			break;
	}
}


void write_conf(CONFIG *conf_p)
{
	int i;
	
	qsort( conf_p->menu, 8, sizeof(MInfo), menu_true_comp );
	qsort( conf_p->item, 9, sizeof(ItemInfo), item_true_comp );
	
	for(i=0; i<9; i++)Data2Str(0, conf_p, i, item[i]);
	for(i=0; i<8; i++)Data2Str(1, conf_p, i, menu[i]);
	
	sceIoRemove(conf_ini);
	ILPWriteToFile(conf_key, conf_ini, "\r\n");
	
	qsort( conf_p->menu, 8, sizeof(MInfo), menu_menu_comp );
	qsort( conf_p->item, 9, sizeof(ItemInfo), item_menu_comp );
	
	SceUID fp = sceIoOpen(conf_bin, PSP_O_RDWR|PSP_O_TRUNC|PSP_O_CREAT, 0777);
	if(fp > 0)
	{
		sceIoWrite(fp, conf_p, sizeof(CONFIG));
	}
	sceIoClose(fp);
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  iniSetting
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define DATE_FORMAT "%weekday %monthnum/%day/%year %hour24:%min:%sec by ABCanG"
//--------------------
void make_lang(CONFIG *conf_p)
{
	int i, j;
	char lang_ini[128];
	char lang_bin[128];
	
	char *buf = mem_alloc(LANG_KEY_NUM * 256);
	char *lang_str[LANG_KEY_NUM];
	
	sprintf(lang_ini, "%s%s", prxPath, LANG_INI);
	sprintf(lang_bin, "%s%s", prxPath, LANG_BIN);
	
	for(lang_str[0] = buf, i=1; i<LANG_KEY_NUM; i++)
	{
		lang_str[i] = lang_str[i-1] + 256;
	}
	
	// iniファイルの読み込みの下準備
	ILPInitKey(lang_key);
	
	ILPRegisterString(lang_key, "Date", lang_str[0], DATE_FORMAT);
	
	// 曜日関連の設定
	ILPRegisterString(lang_key, "Sun", lang_str[1], "Sunday");
	ILPRegisterString(lang_key, "Mon", lang_str[2], "Monday");
	ILPRegisterString(lang_key, "Tue", lang_str[3], "Tuesday");
	ILPRegisterString(lang_key, "Wed", lang_str[4], "Wednesday");
	ILPRegisterString(lang_key, "Thu", lang_str[5], "Thursday");
	ILPRegisterString(lang_key, "Fri", lang_str[6], "Friday");
	ILPRegisterString(lang_key, "Sat", lang_str[7], "Saturday");

	// 月関連の設定
	ILPRegisterString(lang_key, "Jan", lang_str[8], "January");
	ILPRegisterString(lang_key, "Feb", lang_str[9], "February");
	ILPRegisterString(lang_key, "Mar", lang_str[10], "March");
	ILPRegisterString(lang_key, "Apr", lang_str[11], "April");
	ILPRegisterString(lang_key, "May", lang_str[12], "May");
	ILPRegisterString(lang_key, "Jun", lang_str[13], "June");
	ILPRegisterString(lang_key, "Jul", lang_str[14], "July");
	ILPRegisterString(lang_key, "Aug", lang_str[15], "August");
	ILPRegisterString(lang_key, "Sep", lang_str[16], "September");
	ILPRegisterString(lang_key, "Oct", lang_str[17], "October");
	ILPRegisterString(lang_key, "Nov", lang_str[18], "November");
	ILPRegisterString(lang_key, "Dec", lang_str[19], "December");

	// 言語関連の設定
	ILPRegisterString(lang_key, "ExitGame",		lang_str[20], "Exit Game");
	ILPRegisterString(lang_key, "ResetGame",	lang_str[21], "Reset Game");
	ILPRegisterString(lang_key, "Shutdown",		lang_str[22], "Shutdown");
	ILPRegisterString(lang_key, "Standby",		lang_str[23], "Standby");
	ILPRegisterString(lang_key, "Pause",		lang_str[24], "Pause");
	ILPRegisterString(lang_key, "Resume",		lang_str[25], "Resume");
	ILPRegisterString(lang_key, "Back",			lang_str[26], "Back");
	ILPRegisterString(lang_key, "ConnectUSB",	lang_str[27], "ConnectUSB");
	ILPRegisterString(lang_key, "DisconnectUSB",lang_str[28], "DisconnectUSB");
	ILPRegisterString(lang_key, "PSPStates",	lang_str[29], "PSPStates");

	ILPRegisterString(lang_key, "message_ExitGame",	lang_str[30], "Exit Game");
	ILPRegisterString(lang_key, "message_ResetGame",lang_str[31], "Reset Game");
	ILPRegisterString(lang_key, "message_Shutdown",	lang_str[32], "Shutdown");
	ILPRegisterString(lang_key, "message_Standby",	lang_str[33], "Standby");
	ILPRegisterString(lang_key, "message_Check",	lang_str[34], "Are you sure?");

	if(ILPReadFromFile(lang_key, lang_ini))
	{
		sceIoRemove(lang_ini);
	}
	ILPWriteToFile(lang_key, lang_ini, "\r\n");
	
	char *tmp = strchr(lang_str[0], '\0');
	
	for(i=1, tmp++; i<LANG_KEY_NUM; i++, tmp++)
	{
		for( j=0; lang_str[i][j] != '\0'; j++, tmp++) *tmp = lang_str[i][j];
		*tmp = '\0';
	}
	
	SceUID fp = sceIoOpen(lang_bin, PSP_O_RDWR|PSP_O_TRUNC|PSP_O_CREAT, 0777);
	if(fp > 0)
	{
		sceIoWrite(fp, buf, tmp-buf);
	}
	sceIoClose(fp);
	mem_free(buf);
}



void Str2Data(int type, CONFIG *conf_p, int num, char *str)
{
	char *data[16];
	int i;
	
	ILPRemoveSpaceAndNewline(str);
	
	for(i=1, data[0] = str; data[i-1] != NULL; i++)
	{
		data[i] = ILPChToken(data[i-1], ",");
	}
	
	switch(type)
	{
		case 0:
			conf_p->item[num].true_num = num;
			conf_p->item[num].menu_num = atoi(data[0]);
			conf_p->item[num].bar_num = atoi(data[1]);
			if(conf_p->item[num].bar_num < 0 || conf_p->item[num].bar_num > 3)
				conf_p->item[num].bar_num = 0;
			conf_p->item[num].x_addr = atoi(data[2]);
			conf_p->item[num].right = (atoi(data[3]) ? true : false);
			conf_p->item[num].view = (atoi(data[4]) ? true : false);
			break;
		
		case 1:
			conf_p->menu[num].true_num = num;
			conf_p->menu[num].menu_num = atoi(data[0]);
			conf_p->menu[num].view = (atoi(data[1]) ? true : false);
			break;
	}
}

bool once_print[9] = {
	0,1,0,1,1,0,0,0,0
};

void init_conf(CONFIG *conf_p)
{
	// iniファイルの読み込みの下準備
	ILPInitKey(conf_key);
	
	ILPRegisterBool(conf_key, "SwapButton",	&conf_p->swap, false);
	ILPRegisterBool(conf_key, "Mute",		&conf_p->mute, false);
	ILPRegisterBool(conf_key, "StopGame",	&conf_p->stop, false);
	ILPRegisterBool(conf_key, "DoubleColor",&conf_p->color.twin, true);
	ILPRegisterBool(conf_key, "Menu_pos",	&menu_print_pos, true);
	
	ILPRegisterHex(conf_key, "MainBackColor",	&conf_p->color.main_b, BLACK);
	ILPRegisterHex(conf_key, "MainFontColor",	&conf_p->color.main_f, WHITE);
	ILPRegisterHex(conf_key, "NormalBackColor1",	&conf_p->color.nom_b1, BLACK);
	ILPRegisterHex(conf_key, "NormalBackColor2",	&conf_p->color.nom_b2, SILVER);
	ILPRegisterHex(conf_key, "NormalFontColor1",	&conf_p->color.nom_f1, WHITE);
	ILPRegisterHex(conf_key, "NormalFontColor2",	&conf_p->color.nom_f2, GRAY);
	ILPRegisterHex(conf_key, "ActiveBackColor1",&conf_p->color.act_b1, GRAY);
	ILPRegisterHex(conf_key, "ActiveFontColor2",&conf_p->color.act_f1, GREEN);
	
	ILPRegisterDec(conf_key, "BarType",		&bartype, 6);
	
	ILPRegisterString(conf_key, "CPU/BUS",		item[0], "8,3,116,0,1");
	ILPRegisterString(conf_key, "Description",	item[1], "7,3,4,0,1");
	ILPRegisterString(conf_key, "Brightness",	item[2], "6,2,36,0,1");
	ILPRegisterString(conf_key, "Gameid",		item[3], "5,2,478,1,1");
	ILPRegisterString(conf_key, "Gamename",		item[4], "4,3,478,1,1");
	ILPRegisterString(conf_key, "Volume",		item[5], "3,2,86,0,1");
	ILPRegisterString(conf_key, "VolGauge",		item[6], "2,2,360,1,1");
	ILPRegisterString(conf_key, "Date",			item[7], "1,0,440,1,1");
	ILPRegisterString(conf_key, "Battery",		item[8], "0,0,0,0,1");
	
	ILPRegisterString(conf_key, "ExitGame",	menu[0], "0,1");
	ILPRegisterString(conf_key, "ResetGame",menu[1], "1,1");
	ILPRegisterString(conf_key, "Shutdown",	menu[2], "2,1");
	ILPRegisterString(conf_key, "Stanbay",	menu[3], "3,1");
	ILPRegisterString(conf_key, "Pause",	menu[4], "4,1");
	ILPRegisterString(conf_key, "Back",		menu[5], "5,1");
	ILPRegisterString(conf_key, "ConectUSB",menu[6], "6,1");
	ILPRegisterString(conf_key, "PSPStates",menu[7], "7,1");
}

void make_conf(CONFIG *conf_p)
{
	int i;
	
	conf_p->version = VERSION;
	
	if(ILPReadFromFile(conf_key, conf_ini))
	{
		sceIoRemove(conf_ini);
	}
	ILPWriteToFile(conf_key, conf_ini, "\r\n");
	
	for(i=0; i<9; i++)Str2Data(0, conf_p, i, item[i]);
	for(i=0; i<8; i++)Str2Data(1, conf_p, i, menu[i]);
	for(i=0; i<4; i++)conf_p->bar[i] = bar_ptrn[bartype][i];
	for(i=0, conf_p->menu_itm=0; i<8; i++)
	{
		if(conf_p->menu[i].view)conf_p->menu_itm++;
	}
	
	conf_p->menu_pos = (menu_print_pos ? (conf_p->bar[0] + conf_p->bar[1])*10 :
			272-(conf_p->bar[2] + conf_p->bar[3] )*10 - (conf_p->menu_itm*9+4));
	
	qsort( conf_p->menu, 8, sizeof(MInfo), menu_menu_comp );
	qsort( conf_p->item, 9, sizeof(ItemInfo), item_menu_comp );
	
	SceUID fp = sceIoOpen(conf_bin, PSP_O_RDWR|PSP_O_TRUNC|PSP_O_CREAT, 0777);
	if(fp > 0)
	{
		sceIoWrite(fp, conf_p, sizeof(CONFIG));
	}
	sceIoClose(fp);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  iniSetting
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//#define DATE_FORMAT "%weekday %monthnum/%day/%year %hour24:%min:%sec"
//--------------------
void iniSetting(CONFIG *conf_p)
{
	char lang_ini[128];
	char lang_bin[128];
	int ret[2];
	SceIoStat stat[2];
	SceUID fp;
	
	sprintf(conf_ini, "%s%s", prxPath, CONF_INI);
	sprintf(conf_bin, "%s%s", prxPath, CONF_BIN);
	sprintf(lang_bin, "%s%s", prxPath, LANG_BIN);
	sprintf(lang_ini, "%s%s", prxPath, LANG_INI);
	
	memset(conf_p, 0, sizeof(CONFIG));
	
	init_conf(conf_p);
	
	//Load conf
	ret[0] = sceIoGetstat(conf_ini, &stat[0]);
	ret[1] = sceIoGetstat(conf_bin, &stat[1]);
	
	//Not found or time(to minutes) not match
	if(ret[0] < 0 || ret[1] < 0 || memcmp(&stat[0].st_mtime, &stat[1].st_mtime, sizeof(u16)*5) != 0)
	{
		make_conf(conf_p);
	}
	
	fp = sceIoOpen(conf_bin, PSP_O_RDONLY, 0777);
	if(fp > 0)
	{
		sceIoRead(fp, conf_p, sizeof(conf));
	}
	sceIoClose(fp);
	
	if(conf_p->version != VERSION )
	{
		make_conf(conf_p);
		make_lang(conf_p);
		
		fp = sceIoOpen(conf_bin, PSP_O_RDONLY, 0777);
		if(fp > 0)
		{
			sceIoRead(fp, conf_p, sizeof(conf));
		}
		sceIoClose(fp);
	}
	
	
	//Load lang
	char *tmp = NULL, *str_buf = NULL;
	int i, j, len, num[4] = {7, 12, 10, 5};
	
	ret[0] = sceIoGetstat(lang_ini, &stat[0]);
	ret[1] = sceIoGetstat(lang_bin, &stat[1]);
	
	//Not found or time(to minutes) not match
	if(ret[0] < 0 || ret[1] < 0 || memcmp(&stat[0].st_mtime, &stat[1].st_mtime, sizeof(u16)*5) != 0)
	{
		make_lang(conf_p);
	}
	
	fp = sceIoOpen(lang_bin, PSP_O_RDONLY, 0777);
	if(fp < 0)
	{
		sceIoClose(fp);
		return;
	}
	int size = stat[1].st_size;
	mem_set_alloc_mode(MEM_KERN);
	str_buf = mem_alloc(size);
	mem_set_alloc_mode(MEM_AUTO);
	if(str_buf == NULL){
		sceIoClose(fp);
		mem_free(str_buf);
		return;
	}
	int read_size = sceIoRead(fp, str_buf, size);
	sceIoClose(fp);
	if(read_size != size)
	{
		mem_free(str_buf);
		return;
	}
	
	tmp = str_buf;
	conf_p->menu_len = 0;
	string.date = tmp;
	for(j=0; j<4; j++)
	{
		for(i=0; i<num[j]; i++)
		{
			tmp = 1+strchr(tmp, '\0');
			switch(j)
			{
				case 0:
					string.week[i] = tmp;
					break;
				case 1:
					string.month[i] = tmp;
					break;
				case 2:
					string.lang[i] = tmp;
					len = libmLen(tmp)*8;
					if(conf_p->menu_len < len)conf_p->menu_len = len;
					
					break;
				case 3:
					string.message[i] = tmp;
					break;
			}
		}
	}
	
}


void getMenu_pos_flag(CONFIG *conf_p)
{
	if(conf_p->menu_pos == (272-(conf_p->bar[2] + conf_p->bar[3] )*10 - (conf_p->menu_itm*9+4)))
	{
		menu_print_pos = 0;
	} else {
		menu_print_pos = 1;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  SettingMethod
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void SettingMethod(CONFIG *conf_p)
{
	SceCtrlData pad;
	setting_pos = 0;
	setting_flag = true;
	make_conf(conf_p);
	getMenu_pos_flag(conf_p);
	cpMainSettingStr(conf_p);
	PrintMainSetting(7, setting_pos, conf_p, &dinfo);
	libctrlWaitButtonUp(PSP_CTRL_SELECT);
	
	while(setting_flag)
	{
		sceKernelDelayThread(5000);
		sceCtrlPeekBufferPositive(&pad, 1);
		GetButtons(&pad, 500*1000, 150*1000, SettingButton, conf_p);
	}
	write_conf(conf_p);
}

//extensionには「.」から入れること
int checkExtension(const char *path, const char *extension){
	int path_len = strlen(path);
	int extension_len = strlen(extension);
	int pos = path_len - extension_len;
	if(pos < 0)return -1;
	return stricmp(&path[pos], extension);
}


int getRandomBMPName(char *path){
	SceUID dp;
	SceIoDirent entry;
	int num = 0;

	dp = sceIoDopen(path);
	if(dp < 0){
		path[0] = '\0';
		return -1;
	}
	memset(&entry, 0, sizeof(entry));
	while(sceIoDread(dp, &entry) > 0){
		if((strcmp(&entry.d_name[0], ".") == 0) || (strcmp(&entry.d_name[0], "..") == 0) || checkExtension(entry.d_name, ".bmp"))continue;
			num++;
	}
	sceIoDclose(dp);
	if(num == 0){
		path[0] = '\0';
		return -1;
	}
	
	num = sceKernelLibcClock()%num;
	
	dp = sceIoDopen(path);
	if(dp >= 0){
		memset(&entry, 0, sizeof(entry));
		while(sceIoDread(dp, &entry) > 0){
			if((strcmp(&entry.d_name[0], ".") == 0) || (strcmp(&entry.d_name[0], "..") == 0) || checkExtension(entry.d_name, ".bmp"))continue;
			if(num == 0){
				strcat(path, "/");
				strcat(path, entry.d_name);
				sceIoDclose(dp);
				return 0;
			}
			num--;
		}
	}
	sceIoDclose(dp);
	path[0] = '\0';
	return -1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  GetInfo  
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void *sceKernelGetGameInfo();

int GetInfo(void *argp)
{
	char *temp;
	int i;
	int api = (kuKernelInitApitype() & 0xff0);
	
	model = sceKernelGetModel();
	gameid = sceKernelGetGameInfo()+0x44;
	gamepath = sceKernelGetGameInfo()+ 0x74;
	
	//読み込むiniファイルのパスを決定
	strcpy(prxPath, argp);
	temp = strrchr(prxPath, '/');
	if( temp != NULL ) *temp = '\0';
	
	for(i=0; prxPath[i] != ':'; i++)
	{
		drive[i] = prxPath[i];
	}
	drive[i] = '\0';
	
	//back ground image path
	strcpy(imagePath, argp);
	temp = strrchr(imagePath, '/');
	if( temp != NULL ) *temp = '\0';
	strcat(imagePath, "/image");
	getRandomBMPName(imagePath);
	
	return GetGameTitle(gamename, (api == 0x120) ? UMDSFO : gamepath);
}


int (*GoSaveState)(int unk);

int getGoSaveStateAddr()
{
	u32 text_addr, text_end;
	GoSaveState = NULL;
	if(model != 4)return -1;
	SceModule2 *mod = sceKernelFindModuleByName("sceImpose_Driver");
	if(mod == NULL)return -1;
	
	text_addr = mod->text_addr;
	text_end = mod->text_addr + mod->text_size;
	
	for(; text_addr < text_end; text_addr += 4)
	{
		if(_lw(text_addr + 16) == 0xACC200E0)
		{
			break;
		}
	}
	if(text_addr < text_end)
	{
		GoSaveState = (void *)text_addr;
		return 0;
	}
	return -1;
}



void ExitGame()
{
	SceCtrlData pad;
	sceCtrlPeekBufferPositive(&pad, 1);
	myResumeThread();
	if(model == 4 && pad.Buttons & PSP_CTRL_LTRIGGER){
		GoSaveState(1);
	} else {
		sceKernelExitVSHVSH(0);
	}
}

void ResetGame()
{
	LoadExecVSH(kuKernelInitApitype(), gamepath);
}

void Shutdown()
{
	scePowerRequestStandby();
}

void Stanbay()
{
	cusHomeClose();
	scePowerRequestSuspend();
}

void Pause()
{
	StopGame ^= true;
	if(StopGame)
	{
		mySuspendThread();
	} else {
		myResumeThread();
	}
}

void Back()
{
	cusHomeClose();
}

void ConectUSB()
{
	if(!(conf.stop || StopGame) )
	{
		if(usb == 0)
		{
			mySuspendThread();
		} else {
			myResumeThread();
		}
	}
	usb = connect_usb();
	PrintMainMenu(&conf, &dinfo);

}

void PSPStates()
{
	if(!( conf.stop )&& !( StopGame ))
	{
		mySuspendThread();
	}
	StatesMethod(&dinfo);
	if(!( conf.stop )&& !( StopGame ))
	{
		myResumeThread();
	}
}

void memu_func(int type)
{
	switch(type)
	{
		case 0:return ExitGame();
		case 1:return ResetGame();
		case 2:return Shutdown();
		case 3:return Stanbay();
		case 4:return Pause();
		case 5:return Back();
		case 6:return ConectUSB();
		case 7:return PSPStates();
	}
}


int MenuButton(bool check, int button, void *ptr)
{
	CONFIG *conf_p = ptr;
	
	if(button & PSP_CTRL_HOME)
	{
		if(check)return 1;
		libctrlWaitButtonUp(PSP_CTRL_HOME);
		menu_flag ^= true;
		if(menu_flag)
		{
			if(*tempar != home_col)
			{
				menu_flag = false;
				return 0;
			}
			libctrlMaskAllButtonOn(&maskflag);
			if(conf_p->mute)
			{
				libaudioIoDisable(&muteflag);
			}
			menu_pos = 0;
			while(1)
			{
				if(conf_p->menu[menu_pos].view)break;
				menu_pos += 1;
				if(menu_pos > 7) menu_pos = 0;
			}
			sceDisplaySetFrameBufferInternal(1, dinfo.vinfo->buffer, 512, PSP_DISPLAY_PIXEL_FORMAT_5551, 1);
			draw_image(imagePath, dinfo.vinfo->buffer);
			initPrintStatusBar(&conf, &dinfo);
			PrintMainMenu(&conf, &dinfo);
			//Threads need to be stopped after drawing the menu to prevent hanging in some situations
			if( conf_p->stop )
			{
				mySuspendThread();
			}
		} else {
			menu_flag = true;
			cusHomeClose();
		}
		return 0;
	}
	
	if(menu_flag && button & backKey)
	{
		if(check)return 1;
		libctrlWaitButtonUp(backKey);
		cusHomeClose();
		return 0;
	}

	if(menu_flag)
	{
		if(button & (PSP_CTRL_UP|PSP_CTRL_DOWN))
		{
			if(check)return 1;
			//UP -> -1, DOWN -> +1
			int updown = (((int)((button&(PSP_CTRL_UP|PSP_CTRL_DOWN))-40))/24);
			while(1)
			{
				menu_pos += updown;
				if(menu_pos < 0) menu_pos = 7;
				if(menu_pos > 7) menu_pos = 0;
				if(conf_p->menu[menu_pos].view)break;
			}
			PrintMainMenu(&conf, &dinfo);
			return 0;
		}

		else if(button & PSP_CTRL_RTRIGGER)
		{
			if(check)return 1;
			
			if (currclockspeed < MAXCPULIST){
				currclockspeed++;
			}else{
				currclockspeed = 1;
			}
			scePowerSetClockFrequency(cpu_list[currclockspeed], cpu_list[currclockspeed], bus_list[currclockspeed]);

			PrintMainMenu(&conf, &dinfo);
			return 0;
		}

		else if(button & PSP_CTRL_LTRIGGER)
		{
			if(check)return 1;
			
			if (currclockspeed > 1){
				currclockspeed--;
			}else{
				currclockspeed = MAXCPULIST;
			}
			scePowerSetClockFrequency(cpu_list[currclockspeed], cpu_list[currclockspeed], bus_list[currclockspeed]);

			PrintMainMenu(&conf, &dinfo);
			return 0;
		}
		
		else if(button & PSP_CTRL_SELECT)
		{
			if(check)return 1;
			if(libctrlCountButtons(PSP_CTRL_SELECT, 2))
			{
				if(!( conf_p->stop )&& !( StopGame ))
				{
					mySuspendThread();
				}
				PrintMainMenu(&conf, &dinfo);
				SettingMethod(conf_p);
				if(!( conf_p->stop )&& !( StopGame ))
				{
					myResumeThread();
				}
			}
			return 0;
		}
		else if((button & PSP_CTRL_START)&&!(conf_p->stop))
		{
			if(check)return 1;
			StopGame ^= true;
			//全ボタンが放されるまでwait
			libctrlWaitButtonUp(PSP_CTRL_START);
			if(StopGame)
			{
				mySuspendThread();
			} else {
				myResumeThread();
			}
			PrintMainMenu(&conf, &dinfo);
			return 0;
		}
		
		else if(button & selectKey)
		{
			if(check)return 1;
			int ret = 1;
			libctrlWaitButtonUp(selectKey);
			
			switch(conf_p->menu[menu_pos].true_num)
			{
				case 0:
				case 1:
				case 2:
				case 3:
					strcpy(dialog_str, string.message[conf_p->menu[menu_pos].true_num]);
					ret = DialogMethod();
					if(ret == -1)
					{
						cusHomeClose();
					}
					break;
			}
			if(ret == 1)memu_func(conf_p->menu[menu_pos].true_num);
			Redraw();
			return 0;
		}
	}
	return -1;
}

#define COPY_BUFFERSIZE			512*32
//Thanks Dadrfy
int Copy_File(const char *in, const char *out)
{
	if( strcmp(in,out) == 0 || in == NULL || out == NULL )
		return -1;

	SceUID fd_out = sceIoOpen( out, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC , 0777 );
	if (fd_out < 0){
		sceIoClose(fd_out);
		return -1;
	}
	SceUID fd_in = sceIoOpen( in , PSP_O_RDONLY, 0777);
	if (fd_in < 0){
		sceIoClose(fd_in);
		sceIoClose(fd_out);
		return -1;
	}
	
//	unsigned char buffer[COPY_BUFFERSIZE];
	unsigned char *buffer = mem_alloc(COPY_BUFFERSIZE);

	while(1)
	{
		int read_byte = sceIoRead(fd_in, buffer, COPY_BUFFERSIZE);
		if( read_byte < 0 ) {
			sceIoClose(fd_in);
			sceIoClose(fd_in);
			return -1;
		} else if(read_byte == 0 ) {
			break;
		} else if(read_byte == COPY_BUFFERSIZE ) {
			sceIoWrite(fd_out, buffer, COPY_BUFFERSIZE );
		} else {
			sceIoWrite(fd_out, buffer, read_byte);
			break;
		}
	}

	sceIoClose(fd_out);
	sceIoClose(fd_in);
	mem_free(buffer);
	return 0;
}


void cushome_update(void *argp)
{
	char path[256];
	char buf[256];
	SceIoStat stat;
	
	sprintf(path, "%s:/PSP/COMMON", drive); 
	sprintf(buf, "%s/cushome.prx", path);
	if(sceIoGetstat(buf, &stat) == 0)
	{
		Copy_File(buf, argp); 
		sceIoRemove(buf);
		sprintf(buf,"%scushome_conf.prx",  path);
		if(sceIoGetstat(buf, &stat) == 0)
		{
			sprintf(path, "%scushome_conf.prx", prxPath);
			Copy_File(buf, path); 
			sceIoRemove(buf);
		}
		LoadExecVSH(kuKernelInitApitype(), gamepath);
	}
	return;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  threadMain
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int threadMain(SceSize args, void *argp)
{
	SceCtrlData pad;
	sceCtrlPeekBufferPositive(&pad, 1);
	while( sceKernelFindModuleByName( "sceKernelLibrary" ) == NULL ) sceKernelDelayThread( 1000 );
	//スレッド定義
	SceUID info_thid = sceKernelCreateThread("GetInfo", threadInfo, 16, 0x3000, 0, NULL);
	
	//メニュー表示中に他スレッド停止を有効にするため
	//プラグイン起動時のスレッドをダンプして安全リストを作っておく
	Get_FirstThreads();
	int i;
	//最大30秒待つ
	for(i=0; (sceKernelCheckExitCallback() == 0 && i < 30000); i++){
		sceKernelDelayThread(1000);
		i++;
	}
	
	
	// 起動時にHOMEが押されているorゲームでないなら無効
	if( (pad.Buttons & PSP_CTRL_HOME) ||
		(sceKernelInitKeyConfig() != PSP_INIT_KEYCONFIG_GAME)||
		(sceImposeGetHomePopup() == 0)||
		(sceKernelCheckExitCallback() == 0))
	{
		sceKernelSelfStopUnloadModule(0, 0, NULL);
		return 0;
	}
	
	sceImposeSetHomePopup(0);
	sceImposeSetUMDPopup(0);
	
	//パッチ
	MAKE_JUMP(sctrlHENFindFunction("sceImpose_Driver", "sceImpose", 0x5595A71A), dumy_func);//sceImposeSetHomePopup
	MAKE_JUMP(sctrlHENFindFunction("sceImpose_Driver", "sceImpose", 0x72189C48), dumy_func);//sceImposeSetUMDPopup
	ClearCaches();
	
	cushome_update(argp);
	loadLibraries();
	GetInfo(argp);
	getGoSaveStateAddr();
	
	memset(&dinfo, 0, sizeof(dinfo));
	memset(&vinfo, 0, sizeof(vinfo));
	dinfo.vinfo = &vinfo;
	
	
	// libmenuのFontロード
	libmLoadFont(LIBM_FONT_CG);
	libmLoadFont(LIBM_FONT_ICON);
	libmLoadFont(LIBM_FONT_HANKAKU_KANA);
	libmLoadFont(LIBM_FONT_SJIS);
	
	// iniファイルの設定
	iniSetting(&conf);
	
	//pspstates
	loadStates();
	
	selectKey	= (conf.swap ? PSP_CTRL_CIRCLE : PSP_CTRL_CROSS);	//選択 ×
	backKey		= (conf.swap ? PSP_CTRL_CROSS : PSP_CTRL_CIRCLE);	//戻る ○
	
	
	PspSysEventHandler events;
	//他スレッド停止させたままPSPがサスペンドすると
	//フリーズの原因になるので回避するための処理
	events.size			= sizeof(PspSysEventHandler);
	events.name			= "MSE_Suspend";
	events.type_mask	= 0x00FFFF00;
	events.handler		= Callback_Suspend;
	
	sceKernelRegisterSysEventHandler(&events);
	
	dinfo.convert = NULL;
	dinfo.blend = NULL;
	dinfo.vinfo->width = 480;
	dinfo.vinfo->height = 272;
	dinfo.vinfo->format = PSP_DISPLAY_PIXEL_FORMAT_5551;
	dinfo.vinfo->pixelSize = 2;
	dinfo.vinfo->lineWidth = 512;
	dinfo.vinfo->lineSize = dinfo.vinfo->lineWidth * dinfo.vinfo->pixelSize;
	dinfo.vinfo->frameSize = dinfo.vinfo->lineSize * dinfo.vinfo->height;
	dinfo.vinfo->buffer = (void *)GetHomeAddr();
	tempar = (u16 *)(dinfo.vinfo->buffer + dinfo.vinfo->lineSize - dinfo.vinfo->pixelSize);
	
	init_image(imagePath, dinfo.vinfo->buffer);
	//スレッド開始
	if(info_thid>= 0)sceKernelStartThread(info_thid, args, argp);
	
	while( 1 )
	{
		sceCtrlPeekBufferPositive(&pad, 1);
		
		if(suspendCB)
		{
			sceKernelDelayThread(2*1000*1000);
			suspendCB = false;
		}
		
		GetButtons(&pad, 500*1000, 150*1000, MenuButton, &conf);
		sceKernelDelayThread(5000);
	}
	return sceKernelExitDeleteThread( 0 );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  module_start
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int module_start(SceSize args, void *argp)
{
	SceUID main_thid = sceKernelCreateThread("CustomHOME", threadMain, 16, 0x4000, 0, NULL);
	if(main_thid >= 0)sceKernelStartThread(main_thid, args, argp);
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  module_start
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int module_stop(SceSize args, void *argp)
{
    return 0;
}


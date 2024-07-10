
#include "common.h"

const char *LangYesNo[2][2] =
	{
		{" YES ", "<YES>"},
		{"<N O>", " N O "}
	};


char *ColorList[21] =
{
	"ORIGINAL",
	"BLACK",
	"BLUE",
	"RED",
	"GREEN",
	"DARKBLUE",
	"LIGHTBLUE",
	"BROWN",
	"PINK",
	"DARKGREEN",
	"YELLOGREEN",
	"PURPLE",
	"ORANGE",
	"LEMON",
	"YELLOW",
	"SKYBLUE",
	"PEARLORANGE",
	"GRAY",
	"SILVER",
	"GOLD",
	"WHITE"
};

char SettingLang[15][64];
/*
0	SwapButton
1	Homebrew
2	Mute
3	BatteryWarning
4	Stop Game
5	Brightness
6	Description
7	GameName
8	GameID
9	PspStates
10	CPU/BUS
11	ActiveFontColor
12	NormalFontColor
13	ActiveBackColor
14	NormalBackColor
*/
CONFIG conf;

//Print
char PrintBattery[32];
char PrintGameid[16];
char PrintCPUBUS[32];
char PrintVolGauge[64];
char PrintDate[128];
char PrintBrightness[4];
char PrintVolume[4];
char dialog_str[128];

//その他
pspTime tick_time;
int weekNumber;
int btryLifeP;
int btryLifeT;
int charge_flag;
int volume;
int mute_status;
extern int usb;

int ber_addr[4] = { 0, 10, 252, 262};


u32 color[14];


int mystrcmp(char *str1, char *str2)
{
	return strncmp(str1, str2, strlen(str2));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  DateCopy
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void DateCopy(char *after, char *before)
{
	int count = 0;
	char buf[32];
	char *p = after;
	char *key[10] = {
		"%year", "%monthname",
		"%monthnum", "%day",
		"%weekday", "%hour12",
		"%hour24", "%min",
		"%sec", "%ampm"
	};

	strcpy(after, before);

	while((p = strchr(p, '%')) != NULL)
	{
		for(count=0; count<10; count++)
		{
			if(mystrcmp(p, key[count]) == 0)
			{
				switch (count){
					case 0:
						sprintf(buf, "%d", tick_time.year);
						break;
					case 1:
						sprintf(buf, "%s", string.month[tick_time.month -1]);
						break;
					case 2:
						sprintf(buf, "%02d", tick_time.month);
						break;
					case 3:
						sprintf(buf, "%02d", tick_time.day);
						break;
					case 4:
						sprintf(buf, "%s", string.week[weekNumber]);
						break;
					case 5:
						sprintf(buf, "%02d", tick_time.hour > 11 ? tick_time.hour - 12 : tick_time.hour);
						break;
					case 6:
						sprintf(buf, "%d", tick_time.hour);
						break;
					case 7:
						sprintf(buf, "%02d", tick_time.minutes);
						break;
					case 8:
						sprintf(buf, "%02d", tick_time.seconds);
						break;
					case 9:
						sprintf(buf, (tick_time.hour < 12 ? "AM" : "PM"));
						break;
				}
				StrReplace(p, key[count], buf );
				break;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  PrintSpeakerIcon
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define PrintSpeaker_base( x, y, f_color, b_color, dinfo )	libmPrintSymbolXY( x, y, f_color, b_color, f_color, b_color, "\x06", dinfo )
#define PrintSpeaker_mute( x, y, f_color, b_color, dinfo )	libmPrintSymbolXY( x, y, b_color, f_color, f_color, b_color, "\x06", dinfo )
#define PrintSpeaker_zero( x, y, f_color, b_color, dinfo )	libmPrintSymbolXY( x, y, f_color, b_color, b_color, b_color, "\x07", dinfo )
#define PrintSpeaker_one( x, y, f_color, b_color, dinfo )	libmPrintSymbolXY( x, y, f_color, f_color, b_color, b_color, "\x07", dinfo )
#define PrintSpeaker_two( x, y, f_color, b_color, dinfo )	libmPrintSymbolXY( x, y, f_color, f_color, f_color, b_color, "\x07", dinfo )
//-----------------------------------------------------------------------
void PrintSpeakerIcon(int x, int y, int f_color, int b_color, libm_draw_info *dinfo)
{
	// スピーカーの描画
	PrintSpeaker_base( x, y, f_color, b_color, dinfo );
	
	if(mute_status)
	{
		PrintSpeaker_mute( x+8, y, f_color, b_color, dinfo );
	}
	else if(volume == 0)
	{
		PrintSpeaker_zero( x+8, y, f_color, b_color, dinfo );
	}
	else if(volume <= 15)
	{
		PrintSpeaker_one( x+8, y, f_color, b_color, dinfo );
	}
	else if(volume <= 30)
	{
		PrintSpeaker_two( x+8, y, f_color, b_color, dinfo );
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  PrintBatteryIcon
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define PrintBattery_none( x, y, f_color, b_color, dinfo )	libmPrintSymbolXY( x, y, f_color, f_color, f_color, b_color, "\x03", dinfo )
#define PrintBattery_100( x, y, f_color, b_color, dinfo )	libmPrintSymbolXY( x, y, f_color, color[Color_bat_gre], color[Color_bat_gre], b_color, "\x03", dinfo )
#define PrintBattery_80( x, y, f_color, b_color, dinfo )	libmPrintSymbolXY( x, y, f_color, color[Color_bat_gre], b_color, b_color, "\x03", dinfo )
#define PrintBattery_60( x, y, f_color, b_color, dinfo )	libmPrintSymbolXY( x, y, f_color, color[Color_bat_yel], color[Color_bat_yel], b_color, "\x04", dinfo )
#define PrintBattery_40( x, y, f_color, b_color, dinfo )	libmPrintSymbolXY( x, y, f_color, color[Color_bat_ora], b_color, b_color, "\x04", dinfo )
#define PrintBattery_20( x, y, f_color, b_color, dinfo )	libmPrintSymbolXY( x, y, color[Color_bat_red], color[Color_bat_red], b_color, b_color, "\x05", dinfo )
#define PrintBattery_0( x, y, f_color, b_color, dinfo )		libmPrintSymbolXY( x, y, color[Color_bat_red], b_color, b_color, b_color, "\x05", dinfo )
//-----------------------------------------------------------------------
void PrintBatteryIcon(int x, int y, int f_color, int b_color, libm_draw_info *dinfo)
{
	int batFlag = charge_flag;
	
	if(charge_flag == 1 && tick_time.microseconds < 500000)
	{
		batFlag = 0;
	}

	switch(batFlag)
	{
		case 0:
			if(80<btryLifeP)
			{
				PrintBattery_100( x, y, f_color, b_color, dinfo );
			}
			else if(60<btryLifeP)
			{
				PrintBattery_80( x, y, f_color, b_color, dinfo );
			}
			else if(40<btryLifeP)
			{
				PrintBattery_60( x, y, f_color, b_color, dinfo );
			}
			else if(20<btryLifeP)
			{
				PrintBattery_40( x, y, f_color, b_color, dinfo );
			}
			else if(10<btryLifeP)
			{
				PrintBattery_20( x, y, f_color, b_color, dinfo );
			} else {
				PrintBattery_0( x, y, f_color, b_color, dinfo );
			}
			break;
			
		case 1:
			libmPrintXY( x, y, b_color, b_color, " ", dinfo);
			break;
			
		default:
			PrintBattery_none( x, y, f_color, b_color, dinfo );
			break;
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  PrintDescription
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define PrintOKIcon( x, y, f_color, b_color, dinfo )	\
	libmPrintSymbolXY( x, y, f_color, b_color, b_color, b_color, "\x12", dinfo );
#define PrintReturnIcon( x, y, f_color, b_color, dinfo )	\
	libmPrintSymbolXY( x, y, f_color, b_color, b_color, b_color, "\x13", dinfo );
#define PrintSettingIcon( x, y, f_color, b_color, dinfo )	\
	libmPrintSymbolXY( x, y, f_color, b_color, b_color, b_color, "\x14", dinfo );
#define PrintSelectIcon( x, y, f_color, b_color, dinfo )	\
	libmPrintSymbolXY( x, y, f_color, b_color, b_color, b_color, "\x15\x16\x17", dinfo );

void PriDescription(int x, int y, int f_color, int b_color, libm_draw_info *dinfo)
{
	PrintOKIcon(x, y, f_color, b_color, dinfo );
	libmPrintXY(x+8, y, f_color, b_color, (conf.swap ?":○ " : ":× "), dinfo);
	PrintReturnIcon( x+32, y, f_color, b_color, dinfo );
	libmPrintXY(x+40, y, f_color, b_color, (conf.swap ?":× " : ":○ "), dinfo);
	PrintSettingIcon( x+64, y, f_color, b_color, dinfo );
	libmPrintXY(x+72, y, f_color, b_color, ":", dinfo);
	PrintSelectIcon( x+80, y, f_color, b_color, dinfo );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  PrintVolule
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void PriVolume(int x, int y, int f_color, int b_color, libm_draw_info *dinfo)
{
	libmPrintXY(x, y, f_color, b_color, PrintVolume, dinfo);
	PrintSpeakerIcon(x+16, y, f_color, b_color, dinfo);
}

void PriVolGauge(int x, int y, int f_color, int b_color, libm_draw_info *dinfo)
{
	libmPrintXY(x, y, f_color, b_color, PrintVolGauge, dinfo);
}

#define PrintBrightnessIcon( x, y, b_color, dinfo )	\
	libmPrintSymbolXY( x, y, color[Color_bright], b_color, b_color, b_color, "\x10", dinfo );

void PriBrightness(int x, int y, int f_color, int b_color, libm_draw_info *dinfo)
{
	PrintBrightnessIcon(x, y, b_color, dinfo);
	libmPrintXY(x+8, y, f_color, b_color, PrintBrightness, dinfo);
}

void PriGameid(int x, int y, int f_color, int b_color, libm_draw_info *dinfo)
{
	libmPrintXY(x, y, f_color, b_color, PrintGameid, dinfo);
}

void PriGamename(int x, int y, int f_color, int b_color, libm_draw_info *dinfo)
{
	libmPrintXY(x, y, f_color, b_color, gamename, dinfo);
}

void PriCPUBUS(int x, int y, int f_color, int b_color, libm_draw_info *dinfo)
{
	libmPrintXY(x, y, f_color, b_color, PrintCPUBUS, dinfo);
}

void PriDate(int x, int y, int f_color, int b_color, libm_draw_info *dinfo)
{
	libmPrintXY(x, y, f_color, b_color, PrintDate, dinfo);
}

void PriBattery(int x, int y, int f_color, int b_color, libm_draw_info *dinfo)
{
	PrintBatteryIcon(x, y, f_color, b_color, dinfo);
	libmPrintXY(x+8, y, f_color, b_color, PrintBattery, dinfo);
}

/*
void *PrintFunc[] = {
	PriCPUBUS,
	PriDescription,
	PriBrightness,
	PriGameid,
	PriGamename,
	PriVolume,
	PriVolGauge,
	PriDate,
	PriBattery
};
**/


void *PrintFunc(int type)
{
	switch(type)
	{
		case 0:return PriCPUBUS;
		case 1:return PriDescription;
		case 2:return PriBrightness;
		case 3:return PriGameid;
		case 4:return PriGamename;
		case 5:return PriVolume;
		case 6:return PriVolGauge;
		case 7:return PriDate;
		case 8:return PriBattery;
	}
	return NULL;
}


int right_pos[9];



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  printWindow
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void printWindow(int sx, int sy, int ex, int ey, int frame, int back, libm_draw_info *dinfo)
{
	libmFillRect(sx, sy, ex, ey, back, dinfo);
	libmFrame(sx+1, sy+1, ex-2, ey-2, frame, dinfo);
}

char *GetMenuStr(int num)
{
	switch(num)
	{
		case 4:
			return (StopGame ? string.lang[5] : string.lang[4]);
		case 5:
			return string.lang[6];
		case 6:
			return (usb ? string.lang[8] : string.lang[7]);
		case 7:
			return string.lang[9];
			break;
		default:
			return string.lang[num];
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  PrintMainMenu
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void PrintMainMenu(CONFIG *conf_p, libm_draw_info *dinfo)
{
	int i, count, f_color, b_color;
	int x = 2, y = conf_p->menu_pos+2;
	libmFrame(0, conf_p->menu_pos, conf.menu_len+3, conf.menu_pos + conf.menu_itm*9+3, color[Color_main_b], dinfo);
	libmFrame(1, conf_p->menu_pos+1, conf.menu_len+2, conf.menu_pos + conf.menu_itm*9+2, color[Color_main_b], dinfo);
	for(i=0, count=0; i< 8; i++, y+=9)
	{
		if(conf_p->menu[i].view == false)
		{
			y-=9;
			continue;
		}
		f_color = (menu_pos == i ? color[Color_act_f1] : color[Color_nom_f1]);
		b_color = (menu_pos == i ? color[Color_act_b1] : ( conf_p->color.twin && count&1 ? color[Color_nom_b1] : color[Color_nom_b2]) );
		libmFillRect(x, y, x+conf_p->menu_len, y+9, b_color, dinfo);
		libmPrintXY(x, y+1, f_color, 0, GetMenuStr(conf_p->menu[i].true_num), dinfo);
		count++;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  PrintDialog
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int d_width;

void initPrintDialog(libm_draw_info *dinfo)
{
	int len = libmLen(dialog_str);
	int len2 = libmLen(string.message[4]);
	d_width = (len > len2 ? len : len2);
	d_width = 2*(d_width > 11 ? d_width*4+4 : 11*4+4);
	
	printWindow(240 - d_width/2, 120, 240 + d_width/2, 152, color[Color_main_f], color[Color_main_b], dinfo);
	libmPrintXY(240-(len*4), 124, color[Color_nom_f1], 0, dialog_str, dinfo);
	libmPrintXY(240-(len2*4), 132, color[Color_nom_f1], 0, string.message[4], dinfo);
}

void PrintDialog(libm_draw_info *dinfo)
{
	libmPrintXY(240-5*8-4, 140, (check_flag ? color[Color_act_f1] : color[Color_nom_f1]), color[Color_main_b], LangYesNo[0][check_flag], dinfo);
	libmPrintXY(240+4, 140, (check_flag ? color[Color_nom_f1] : color[Color_act_f1]), color[Color_main_b], LangYesNo[1][check_flag], dinfo);
}


void initPrintStatusBar(CONFIG *conf_p, libm_draw_info *dinfo)
{
	int i;
	for(i=0; i<4; i++)
	{
		if(conf_p->bar[i])
		{
			libmFillRect(0, ber_addr[i], 480, ber_addr[i]+10, color[Color_main_b], dinfo);
		}
	}
	for(i=8; i>=0; i--)
	{
		if(conf_p->bar[conf_p->item[i].bar_num] && conf_p->item[i].view)
		{
			void (* func)() = (void *)(PrintFunc(conf_p->item[i].true_num));
			int xpos = conf_p->item[i].x_addr - (conf_p->item[i].right ? right_pos[conf_p->item[i].true_num] : 0);
			func(xpos, ber_addr[conf_p->item[i].bar_num]+1, color[Color_main_f], 0, dinfo);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  PrintStatusBar
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void PrintStatusBar(CONFIG *conf_p, libm_draw_info *dinfo)
{
	int i;
	for(i=8; i>=0; i--)
	{
		if(conf_p->bar[conf_p->item[i].bar_num] 
			&& conf_p->item[i].view && !once_print[conf_p->item[i].true_num])
		{
			void (* func)() = (void *)(PrintFunc(conf_p->item[i].true_num));
			int xpos = conf_p->item[i].x_addr - (conf_p->item[i].right ? right_pos[conf_p->item[i].true_num] : 0);
			func(xpos, ber_addr[conf_p->item[i].bar_num]+1, color[Color_main_f], color[Color_main_b], dinfo);
		}
	}
}

void setColor()
{
	int i;
	u32 color_8888[14] = {
		0,
		conf.color.main_b,
		conf.color.main_f,
		conf.color.nom_b1,
		conf.color.nom_b2,
		conf.color.nom_f1,
		conf.color.nom_f2,
		conf.color.act_b1,
		conf.color.act_f1,
		GOLD,
		GREEN,
		YELLOW,
		ORANGE,
		RED
	};
	
	for(i=0; i<14; i++)
	{
		color[i] = libmConvert8888_5551( color_8888[i] );
	}
}


int GetBatteryLifePercentGo(){
	int ret = 0;
	int volt = scePowerGetBatteryVolt();
	if(volt >= 4150){//
		ret = 100;
	} else if(volt >= 4100){
		ret = 96;
	} else if(volt >= 4050){
		ret = 93;
	} else if(volt >= 4000){//
		ret = 90;
	} else if(volt >= 3950){
		ret = 81;
	} else if(volt >= 3900){
		ret = 73;
	} else if(volt >= 3850){
		ret = 66;
	} else if(volt >= 3800){//
		ret = 60;
	} else if(volt >= 3750){
		ret = 43;
	} else if(volt >= 3700){//
		ret = 30;
	} else if(volt >= 3650){
		ret = 20;
	} else if(volt >= 3600){//
		ret = 12;
	} else if(volt >= 3550){
		ret = 7;
	} else if(volt >= 3500){//
		ret = 4;
	} else if(volt >= 3450){
		ret = 3;
	} else if(volt >= 3400){//
		ret = 2;
	} else if(volt >= 3350){
		ret = 1;
	} else if(volt >= 3300){//
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  threadInfo
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int threadInfo(SceSize args, void *argp)
{
	int Brightness;
	int i;
	
	DateCopy(PrintDate, string.date);
	btryLifeP = scePowerGetBatteryLifePercent();
	btryLifeT = scePowerGetBatteryLifeTime();
	volume = sceImposeGetParam(PSP_IMPOSE_MAIN_VOLUME);
	sceRtcGetCurrentClockLocalTime( &tick_time );
	weekNumber = sceRtcGetDayOfWeek(tick_time.year, tick_time.month, tick_time.day);
	sprintf(PrintGameid, "ID:%s", gameid);
	sprintf(PrintCPUBUS, "CPU/BUS:%3d/%3d", scePowerGetCpuClockFrequency(), scePowerGetBusClockFrequency());
	
	right_pos[0] = strlen(PrintCPUBUS)*8;
	right_pos[1] = 88;
	right_pos[2] = 32;
	right_pos[3] = strlen(PrintGameid)*8;
	right_pos[4] = libmLen(gamename)*8;
	right_pos[5] = 32;
	right_pos[6] = 240;
	right_pos[7] = libmLen(PrintDate)*8;
	right_pos[8] = 12*8;
	
	setColor();
	
	while(1)
	{
		if(menu_flag)
		{
			if(*tempar != home_col)
			{
				cusHomeClose();
				continue;
			}
			volume = sceImposeGetParam(PSP_IMPOSE_MAIN_VOLUME);
			sceRtcGetCurrentClockLocalTime( &tick_time );
			weekNumber = sceRtcGetDayOfWeek(tick_time.year, tick_time.month, tick_time.day);
			charge_flag = scePowerGetBatteryChargingStatus();
			btryLifeP = scePowerGetBatteryLifePercent();
			if (model == 4){
				btryLifeP = GetBatteryLifePercentGo();
			}
			btryLifeT = scePowerGetBatteryLifeTime();			
			mute_status = sceImposeGetParam(PSP_IMPOSE_MUTE);
			sceDisplayGetBrightness(&Brightness, 0);

			//明るさ,音量
			sprintf(PrintBrightness, "%3d", Brightness);
			sprintf(PrintVolume, "%2d", volume);

			PrintVolGauge[0] = '\0';
			for(i=0; i<volume; i++)
			{
				strcat( PrintVolGauge, "｜");
			}
			for(; i<30; i++)
			{
				strcat( PrintVolGauge, "・");
			}
			
			//バッテリー
			if(!charge_flag)
			{
				if(btryLifeT <= 0)sprintf(PrintBattery, "%3d%%", btryLifeP);
				else sprintf(PrintBattery, "%3d%% (%d:%02d) ", btryLifeP, btryLifeT/60, btryLifeT%60);
			}
			else if(charge_flag)
			{
				sprintf(PrintBattery, "%3d%% (--:--) ", btryLifeP);
			} else {
				sprintf(PrintBattery, " NonBattery ");
			}
			
			DateCopy(PrintDate, string.date);
			
			sprintf(PrintCPUBUS, "CPU/BUS:%3d/%3d", scePowerGetCpuClockFrequency(), scePowerGetBusClockFrequency());
			
			PrintStatusBar(&conf, &dinfo);
		}
		sceKernelDelayThread(100 * 1000);
		//0.1秒ディレイ
	}
  return 0;
}


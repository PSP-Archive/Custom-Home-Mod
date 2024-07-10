
#include "common.h"
#include "states.h"

typedef struct StInfo_{
  States States[2];
  PictData SSData[9];
} StInfo;

extern char prxPath[];
extern char *gameid;
extern char *gamepath;

int savetype=0, slot_pos=0;

char statesid[10];
const char number_txt[10] = "udlrcxqts";

const char *CtrlType[3] =
{
	"SAVE",
	"LOAD",
	"DELETE"
};

const char *SaveType[2] =
{
	"LOCAL",
	"GLOBAL"
};

const char *infoText1 = "L/R Change type";
const char *infoText2 = "%s Save";
const char *infoText3 = "%s Cancel";
const char *infoText4 = "  Load";
const char *infoText5 = "¢ Delete";

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  getBinInfo
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void getBinInfo(StInfo *StInfo)
{
	int i, j, ret;
	SceIoStat stat;
	
	char *setDate(char *time_str, ScePspDateTime *time)
	{
		sprintf(time_str, "%u/%2u/%2u %u:%02u:%02u", time->year, time->month, time->day, time->hour, time->minute, time->second);
		return time_str;
	}
	
	strcpy(StInfo->States[0].id, statesid);
	strcpy(StInfo->States[1].id, "globalstate");
	
	for(j=0; j<2; j++)
	{
		for(i=0; i<9; i++)
		{
			sprintf(StInfo->States[j].bin[i].name, "%s_%c", StInfo->States[j].id, number_txt[i]);
			sprintf(StInfo->States[j].bin[i].path, "%s:/seplugins/cushome/SAVESTATE/%s.bin", drive, StInfo->States[j].bin[i].name);
			ret = sceIoGetstat(StInfo->States[j].bin[i].path, &stat);
			StInfo->States[j].bin[i].flag = (ret < 0 ? false : true);
			if(ret < 0)
			{
				strcpy(StInfo->States[j].bin[i].date, "NOT FOUND");
			} else {
				setDate(StInfo->States[j].bin[i].date, &stat.st_mtime);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  statesGameId
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void statesGameId(char *id)
{
	char *ptr, *id_start;

	if((kuKernelInitApitype() & 0xff0) != 0x120)
	{
		ptr = strrchr(gamepath, '/');
		*ptr = '\0';

		id_start = strrchr(gamepath, '/');
		strncpy(id, id_start + 1, 9);

		id[9] = '\0';
		*ptr = '/';
	} else  {
		strcpy(id, gameid);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  FixStatesId
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int FixStatesId(char *id)
{
	u32 addr, text_addr, text_end;
	SceModule2 *mod = NULL;

	mod = sceKernelFindModuleByName("PspStates");
	if(mod == NULL)
	{
		mod = sceKernelFindModuleByName("sleep_patch");
		if(mod == NULL)return -1;
	}
	
	text_addr = mod->text_addr;
	text_end = mod->text_addr + mod->text_size;
	
	for(; text_addr < text_end; text_addr += 4)
	{
		if(( _lw(text_addr) & 0x24470000) == 0x24470000 && (_lw(text_addr - 8) & 0x3C020000)  == 0x3C020000)
		{
			u32 temp = (_lw(text_addr) & 0xffff);
			addr = (mod->text_addr & 0xffff0000) + temp;
			if((mod->text_addr & 0xffff) >= temp)
			{
				addr += 0x10000;
			}
			memcpy((void *)addr, id, 10);
			return 0;
		}
	}
	return -1;
}

extern bool menu_flag;

#define REDIRECT_FUNCTION(a, f) _sw(0x08000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a); _sw(0, a + 4);

#define MASK_BUTTONS (PSP_CTRL_UP | PSP_CTRL_RIGHT | PSP_CTRL_DOWN | PSP_CTRL_LEFT | PSP_CTRL_LTRIGGER | \
PSP_CTRL_RTRIGGER | PSP_CTRL_TRIANGLE | PSP_CTRL_CIRCLE | PSP_CTRL_CROSS | PSP_CTRL_SQUARE | \
PSP_CTRL_SELECT | PSP_CTRL_START | PSP_CTRL_HOME)

int sceCtrlPeekBufferPositivePatched(SceCtrlData *pad, int count)
{
	return sceCtrlPeekBufferPositive(pad, count);
/*	
	if( ! menu_flag ){
		return sceCtrlPeekBufferPositive(pad, count);
	}
	
	
	int res;
	char number;
	
	// call
	res = sceCtrlPeekBufferPositive(pad, count);
	
	if(run_flag != 0){
		pad->Buttons &= ~MASK_BUTTONS;
		return res;
	}

	
	if( (pad->Buttons & conf.startKey) == conf.startKey ){//menu
		run_flag = 1;
		sceKernelSignalSema(semaid, 1);

//		pad->Buttons = PSP_CTRL_RIGHT | PSP_CTRL_UP;		
		pad->Buttons &= ~MASK_BUTTONS;
	}else if(pad->Buttons & PSP_CTRL_RTRIGGER){// save
		number = -1;		
		GetButtons(pad, &number);
		if(number == -1) return res;
		
		g_save_number = number;
		g_mode_flag = pad->Buttons & PSP_CTRL_SELECT;
		
		run_flag = 2;
		sceKernelSignalSema(semaid, 1);
		
		pad->Buttons &= ~MASK_BUTTONS;
	}else if(pad->Buttons & PSP_CTRL_LTRIGGER){// load
		number = -1;
		GetButtons(pad, &number);
		if(number == -1) return res;
		
		g_save_number = number;
		g_mode_flag = pad->Buttons & PSP_CTRL_SELECT;
		
		run_flag = 3;
		sceKernelSignalSema(semaid, 1);
		
		pad->Buttons &= ~MASK_BUTTONS;
	}
	
	
	
	return res;
*/
}

void (* SaveStatesLsL)(char number, int flag);
void (* LoadStatesLsL)(char number, int flag);

//-------------------------------//
void patchToPspStates(void)
{
	u32 addr = FindProc("PspStates", "PspLsLibrary", 0x743B4BE7);
	if( addr != 0 ){
		REDIRECT_FUNCTION(addr, sceCtrlPeekBufferPositivePatched);
		ClearCaches();
	}
	
    SaveStatesLsL = (void *)FindProc("PspStates", "PspLsLibrary", 0xC118AA99);
    LoadStatesLsL = (void *)FindProc("PspStates", "PspLsLibrary", 0xE34921F7);
}

//-------------------------------//

void patchTo_sleep_patch(SceModule2 *mod)
{
	u32 addr = FindProc("sleep_patch", "PspLsLibrary", 0x743B4BE7);
	if( addr != 0 ){
		REDIRECT_FUNCTION(addr, sceCtrlPeekBufferPositivePatched);
	}else{
		REDIRECT_FUNCTION(mod->text_addr + 0x000009A0, sceCtrlPeekBufferPositivePatched);
	}
	
	ClearCaches();
	
	SaveStatesLsL = (void *)FindProc("sleep_patch", "PspLsLibrary", 0xC118AA99);
	LoadStatesLsL = (void *)FindProc("sleep_patch", "PspLsLibrary", 0xE34921F7);
	
}

//-------------------------------//

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  loadStates
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define STATES_PATH "%s:/seplugins/cushome/pspstates_cushome.prx"
int loadStates()
{
	SceIoStat stat;
	char states_path[64];
	
	//Make sure the directory tree exists
	sprintf(states_path, "%s:/seplugins", drive);	
	sceIoMkdir(states_path, 0777);
	sprintf(states_path, "%s:/seplugins/cushome", drive);	
	sceIoMkdir(states_path, 0777);
	sprintf(states_path, "%s:/seplugins/cushome/SAVESTATE", drive);	
	sceIoMkdir(states_path, 0777);	
		
	if(model == 0)
	{
		int i;
		for(i=0; i<8; i++)
		{
			if(conf.menu[i].true_num == 7)
			{
				if(conf.menu[i].view == true)
				{
					make_conf(&conf);
					conf.menu[i].view = false;
					conf.menu_itm--;
					write_conf(&conf);
				}
				break;
			}
		}
		return -1;
	}
	
	sprintf(states_path, STATES_PATH, drive);
	
	if(sceKernelFindModuleByName("sleep_patch") == NULL && sceKernelFindModuleByName("PspStates") == NULL)
	{
		if(sceIoGetstat(states_path, &stat) < 0)
		{
			return -1;
		}
		LoadStartModule(states_path);
		sceKernelDelayThread(100);
	}

	if( sceKernelFindModuleByName("PspStates") != NULL){
		patchToPspStates();
	}
	
	SceModule2 *mod;
	mod = (SceModule2 *)sceKernelFindModuleByName("sleep_patch");
	if( mod != NULL ){
		patchTo_sleep_patch(mod);
	}

	if(sceKernelFindModuleByName("sleep_patch") == NULL && sceKernelFindModuleByName("PspStates") == NULL)
	{
		int i;
		for(i=0; i<8; i++)
		{
			if(conf.menu[i].true_num == 7)
			{
				if(conf.menu[i].view == true)
				{
					conf.menu[i].view = false;
					conf.menu_itm--;
					write_conf(&conf);
				}
				break;
			}
		}
		return -1;
	} else {
		statesGameId(statesid);
		FixStatesId(statesid);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  loadStatesSS
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void loadStatesSS(StInfo *StInfo)
{
	int i, ret=-1;
	
	for(i=0; i<9; i++)
	{
		if(StInfo->States[savetype].bin[i].flag)
		{
			StInfo->SSData[i].raw = mem_alloc(StInfo->SSData[i].size);
			memset(StInfo->SSData[i].raw, 0, StInfo->SSData[i].size);
			ChangeFormatToBmpFormat(StInfo->States[savetype].bin[i].path);
			ret = loadBmp(StInfo->States[savetype].bin[i].path, &StInfo->SSData[i]);
			ChangeFormatToBinFormat(StInfo->States[savetype].bin[i].path);
		}
		if(ret < 0)
		{
			mem_free(StInfo->SSData[i].raw);
			StInfo->SSData[i].raw = NULL;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  PrintStatesSS
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void PrintStatesSS(int x, int y, StInfo *StInfo, libm_draw_info *dinfo)
{
	if(StInfo->SSData[slot_pos].raw != NULL)
	{
		drawPict5551(x, y, &StInfo->SSData[slot_pos], dinfo);
	} else {
		libmFillRect(x, y, x+SCREEN_SHOT_WIDTH, y+SCREEN_SHOT_HEIGHT, color[Color_main_b], dinfo);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  PrintStatesMenu
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void PrintStatesMenu(StInfo *StInfo, libm_draw_info *dinfo)
{
	int i, y=25;
	char buf[256];
	
	for(i=0; i<9; i++, y+=9)
	{
		u32 f_color = (slot_pos == i ? color[Color_act_f1] : (StInfo->States[savetype].bin[i].flag ? color[Color_nom_f1] : color[Color_nom_f2]));
		u32 b_color = (slot_pos == i ? color[Color_act_b1] : ( conf.color.twin && i%2 ? color[Color_nom_b2] : color[Color_nom_b1]) );
		libmFillRect(50, y, 430, y+9, b_color, dinfo);
		libmPrintXY(100, y+1, f_color, 0, StInfo->States[savetype].bin[i].name, dinfo);
		libmPrintXY(240, y+1, f_color, 0, StInfo->States[savetype].bin[i].date, dinfo);
	}
	
	libmPrintXY(365 - strlen(SaveType[savetype])*4, y+10, color[Color_nom_f1], 0, SaveType[savetype], dinfo);
	libmPrintXY(365 - strlen(infoText1)*4, y+25, color[Color_nom_f1], 0, infoText1, dinfo);
	sprintf(buf, infoText2, conf.swap ? "›" : "~");
	libmPrintXY(365 - strlen(buf)*4, y+35, color[Color_nom_f1], 0, buf, dinfo);
	sprintf(buf, infoText3, !conf.swap ? "›" : "~");
	libmPrintXY(365 - strlen(buf)*4, y+45, color[Color_nom_f1], 0, buf, dinfo);
	libmPrintXY(365 - strlen(infoText4)*4, y+55, color[Color_nom_f1], 0, infoText4, dinfo);
	libmPrintXY(365 - strlen(infoText5)*4, y+65, color[Color_nom_f1], 0, infoText5, dinfo);
	
	PrintStatesSS(50, y+5, StInfo, dinfo);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  StatesMethod
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int StatesButton(bool check, int button, void *Info)
{
	int i;
	int ctrltype, ret = 0;
	
	StInfo *StInfo = Info;
	
	if(button & (PSP_CTRL_UP|PSP_CTRL_DOWN))
	{
		if(check)return 1;
		//UP -> -1, DOWN -> +1
		slot_pos += (button & PSP_CTRL_UP) ? -1 : 1;
		if(slot_pos < 0) slot_pos = 8;
		if(slot_pos > 8) slot_pos = 0;
		PrintStatesMenu(StInfo, &dinfo);
		return 0;
	}
	else if(button & (PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER))
	{
		if(check)return 1;
		savetype ^= true;// (savetype-1)*(-1);
		for(i=0; i<9; i++){
			mem_free(StInfo->SSData[i].raw);
			StInfo->SSData[i].raw = NULL;
		}
		loadStatesSS(StInfo);
		printWindow(40, 20, 440, 252, color[Color_main_f], color[Color_main_b], &dinfo);
		PrintStatesMenu(StInfo, &dinfo);
		libctrlWaitButtonUp(PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER);
		return 0;
	}
	else if(button & (selectKey | PSP_CTRL_TRIANGLE | PSP_CTRL_SQUARE))
	{
		if(check)return 1;
		
		if(button & selectKey){
			ctrltype = 0;
		} else if(button & PSP_CTRL_SQUARE){
			ctrltype = 1;
		} else if(button & PSP_CTRL_TRIANGLE){
			ctrltype = 2;
		} else {
			return 1;
		}
		
		libctrlWaitButtonUp((selectKey | PSP_CTRL_TRIANGLE | PSP_CTRL_SQUARE));
		if(ctrltype == 0 || StInfo->States[savetype].bin[slot_pos].flag == true)
		{
			sprintf(dialog_str, "%s %s", CtrlType[ctrltype], StInfo->States[savetype].bin[slot_pos].name);
			ret = DialogMethod();
			if(ret == 0)
			{
				printWindow(40, 20, 440, 252, color[Color_main_f], color[Color_main_b], &dinfo);
				PrintStatesMenu(StInfo, &dinfo);
			}
		}
		
		if(ret == -1)
		{
			slot_pos=0;
			savetype=0;
			cusHomeClose();
			states_flag = false;
			return 0;
		}
		else if(ret == 1)
		{
			libctrlWaitButtonUp(selectKey);
			switch (ctrltype)
			{
				case 0:
					cusHomeClose();
					sceDisplayWaitVblankStart();
					sceDisplayWaitVblankStart();
					ChangeFormatToBmpFormat(StInfo->States[savetype].bin[slot_pos].path);
					takeScreenShot(StInfo->States[savetype].bin[slot_pos].path);
					mem_free(StInfo);
					SaveStatesLsL(number_txt[slot_pos], savetype);
					break;
				case 1:
					if(StInfo->States[savetype].bin[slot_pos].flag)
					{
						cusHomeClose();
						LoadStatesLsL(number_txt[slot_pos], savetype);
					}
					break;
				case 2:
					if(StInfo->States[savetype].bin[slot_pos].flag)
					{
						sceIoRemove(StInfo->States[savetype].bin[slot_pos].path);
						ChangeFormatToBmpFormat(StInfo->States[savetype].bin[slot_pos].path);
						sceIoRemove(StInfo->States[savetype].bin[slot_pos].path);
						getBinInfo(StInfo);
						mem_free(StInfo->SSData[slot_pos].raw);
						StInfo->SSData[slot_pos].raw = NULL;
						loadStatesSS(StInfo);
						printWindow(40, 20, 440, 252, color[Color_main_f], color[Color_main_b], &dinfo);
						PrintStatesMenu(StInfo, &dinfo);
					}
					break;
			}
		}
		return 0;
	}
	else if(button & backKey || button & PSP_CTRL_HOME)
	{
		if(check)return 1;
		libctrlWaitButtonUp(backKey|PSP_CTRL_HOME);
		if(button & PSP_CTRL_HOME)cusHomeClose();
		states_flag = false;
		return 0;
	}
	return -1;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  StatesMethod
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void StatesMethod(libm_draw_info *dinfo)
{
	SceCtrlData pad;
	StInfo *StInfo = mem_alloc(sizeof(States)*2 + sizeof(PictData)*9);
	int i;
	savetype = 0;
	slot_pos = 0;
	states_flag = true;
	
	memset(StInfo, 0, sizeof(States)*2 + sizeof(PictData)*9);
	getBinInfo(StInfo);

	for(i=0; i<9; i++)
	{
		StInfo->SSData[i].raw = NULL;
		StInfo->SSData[i].size = SCREEN_SHOT_BUFFER_SIZE;
		StInfo->SSData[i].width = SCREEN_SHOT_WIDTH;
		StInfo->SSData[i].height = SCREEN_SHOT_HEIGHT;
	}
	
	loadStatesSS(StInfo);
	printWindow(40, 20, 440, 252, color[Color_main_f], color[Color_main_b], dinfo);
	PrintStatesMenu(StInfo, dinfo);
	
	while(states_flag)
	{
		sceKernelDelayThread(1000);
		sceCtrlPeekBufferPositive(&pad, 1);
		GetButtons(&pad, 500*1000, 150*1000, StatesButton, (void *)StInfo);
	}
	for(i=0; i<9; i++){
		mem_free(StInfo->SSData[i].raw);
		StInfo->SSData[i].raw = NULL;
	}
	mem_free(StInfo);
}


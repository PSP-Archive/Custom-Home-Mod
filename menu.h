#ifndef MENU_H
#define MENU_H

#include "common.h"

int threadInfo(SceSize args, void *argp);

void printWindow(int sx, int sy, int ex, int ey, int frame, int back, libm_draw_info *dinfo);
void initPrintDialog(libm_draw_info *dinfo);
void PrintDialog(libm_draw_info *dinfo);
void PrintMainMenu(CONFIG *conf_p, libm_draw_info *dinfo);
void PrintStatusBar(CONFIG *conf_p, libm_draw_info *dinfo);
void initPrintStatusBar(CONFIG *conf_p, libm_draw_info *dinfo);

void print_u16(int sx, int sy, int ex, int ey, libm_draw_info *orig, libm_draw_info *dinfo);
void setColor();


#endif
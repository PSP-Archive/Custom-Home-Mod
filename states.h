#ifndef __PSPSTATES__
#define __PSPSTATES__

#include "common.h"
#include "pict/pict.h"

typedef struct States_{
  char id[16];
  struct
  {
	char date[32];
	char name[16];
    char path[64];
	bool flag;
  }bin[9];
} States;


void PspLsLibrarySaveStates(char number, int flag);
void PspLsLibraryLoadStates(char number, int flag);

// function
int loadStates();
void StatesMethod(libm_draw_info *dinfo);


#endif


//Thanks takka!!

#ifndef GETNAME_H_

#define GETNAME_H_


#include <psptypes.h>
#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
//#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <unistd.h>

/*---------------------------------------------------------------------------
  ISO/CSO/PBPからGAME名取得
  char* title	: ゲーム名のポインタ
  char* path	: PARAM.SFO/自作ソフトのパス
  return int    : 成功時0, エラーの場合は 負の値 を返す
---------------------------------------------------------------------------*/
int GetGameTitle(char *title, const char *path);


#endif /* GETNAME_H_ */

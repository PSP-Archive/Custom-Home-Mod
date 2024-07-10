/*
 * memory.c ver. 1.1
 *
 *  Created on: 2011/09/03
 *      Author: noname
 */

#include <pspkernel.h>
#include <kubridge.h>
#include "memory.h"

static int mem_mode = 0;
static int mem_partition = 0;


/*---------------------------------------------------------------------------
  メモリ確保する場所の設定
    int mode: これから確保するメモリのパーティションを指定、
              0の場合は自動判定するが、動作が遅くなる

    return: 設定値を返す
---------------------------------------------------------------------------*/

int mem_set_alloc_mode(int part)
{
  mem_mode = part;

  return part;
}


/*---------------------------------------------------------------------------
  優先順位番号からパーティション番号を取得
    int num : 優先順位 0～5

    return : パーティション番号
---------------------------------------------------------------------------*/
static int mem_get_auto_partition(int num)
{
  int partitionid;
  int tid;
  SceKernelThreadInfo tinfo;

  // Model
  // 0 : 1000
  // 1 : 2000
  // 2 : 3000
  // 3 : 3000
  // 4 : go
  // 5 : 3000?
  // 6 : 3000?
  // 7 : 3000?
  // 8 : 3000

#define TABLE_LEN 7

  // 冗長かもしれないが、今後のことを考えて全てテーブル化
  int partition_kern[2][3][9][TABLE_LEN] = {
      { // 5.xx
          { // VSH
              { 2, 5, 4, 1, 0, 0, 0}, // fat  9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // go   9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
          },
          { // GAME
              { 1,  2, 0, 0, 0, 0, 0}, // fat  9/15 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // go   9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
          },
          { // POPS
              { 4,  1, 2, 0, 0, 0, 0}, // fat  9/2 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // go   9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
              { 8, 10, 4, 1, 2, 0, 0}, // slim 9/3 fix
          },
      },
      { // 6.xx
          { // VSH
              { 2, 5, 4, 1, 0, 0, 0}, // fat  9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // go   9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
              { 2, 5, 4, 1, 0, 0, 0}, // slim 9/2 fix
          },
          { // GAME
              { 1,  2, 0, 0, 0, 0, 0}, // fat  9/15 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // go   9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
          },
          { // POPS
              { 4,  1, 2, 0, 0, 0, 0}, // fat  9/3 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // go   9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
              { 9, 11, 8, 4, 1, 2, 0}, // slim 9/2 fix
          },
      },
  };

  int partition_user = MEM_USER;

  int fw;
  int model;
  int mode;

  if(mem_mode == MEM_AUTO)
  {
    tid = sceKernelGetThreadId();
    tinfo.size = sizeof(SceKernelThreadInfo);
    sceKernelReferThreadStatus(tid, &tinfo);

    if((tinfo.attr & PSP_THREAD_ATTR_USER) != PSP_THREAD_ATTR_USER)
    {
      // カーネルモード
      fw = (sceKernelDevkitVersion() >> 24) - 5;  // 0 = ～5.xx, 1 = 6.xx
      mode = (sceKernelInitKeyConfig() >> 8) - 1; // 0 = VSH, 1 = GAME, 2 = POPS
      model = kuKernelGetModel();                 // 0 = 1000, 1 = 2000, 2,3,5,6,7,8 = 3000, 4 = go

      if(num < TABLE_LEN)
        partitionid = partition_kern[fw][mode][model][num];
      else
        partitionid =  0;
    }
    else
    {
      // ユーザーモード
      if(num == 0)
        partitionid = partition_user;
      else
        partitionid = 0;
    }
  }
  else
  {
    if(num == 0)
      partitionid = mem_mode;
    else
      partitionid = 0;
  }

  return partitionid;
}

/*---------------------------------------------------------------------------
  メモリ確保するパーティションの読取
    return: 設定値を返す
---------------------------------------------------------------------------*/
/*
int mem_get_alloc_mode(void)
{
  return mem_mode;
}
*/

/*---------------------------------------------------------------------------
  直前に確保したメモリのパーティションを読取
    return : パーティション番号, 未取得の場合は0を返す
---------------------------------------------------------------------------*/
/*
int mem_get_alloc_partition(void)
{
  return mem_partition;
}
*/

/*---------------------------------------------------------------------------
  メモリ確保
    int size: 利用メモリサイズ

    return: 確保したメモリへのポインタ
            エラーの場合はNULLを返す
---------------------------------------------------------------------------*/
void* mem_alloc(int size)
{
  SceUID* p;
  int h_block;
  int partitionid;
  int loop = 0;

  if(size == 0) return NULL;

  while(partitionid = mem_get_auto_partition(loop), partitionid != 0)
  {
    h_block = sceKernelAllocPartitionMemory(partitionid, "mem_alloc", PSP_SMEM_Low, size + sizeof(SceUID*), NULL);
    if(h_block > 0)
      break;
    loop++;
  }
  if(partitionid == 0)
  {
    mem_partition = 0;
    return NULL;
  }

  mem_partition = partitionid;
  p = sceKernelGetBlockHeadAddr(h_block);
  *p = h_block;

  return (void *)(p + 1);
}

/*---------------------------------------------------------------------------
  アライメントを指定したメモリ確保
    int align : アライメントサイズ(2^nであること)
    int size  : 利用メモリサイズ

    return: 確保したメモリへのポインタ
            エラーの場合はNULLを返す
---------------------------------------------------------------------------*/
/*
void* mem_align(int align, int size)
{
  //
  //  *--------------------------合計確保サイズ-----------------------------+
  //  +-free用領域-+-----------必要サイズ----------+-----アライメント用-----+
  //  +--空き--+-free用領域-+---アライメント指定後の必要サイズ---+---空き---+
  //
  SceUID *p;
  SceUID *align_mem;

  p = mem_alloc(size + align + sizeof(SceUID*));
  if(p == NULL)
    return NULL;

  // 確保したメモリ位置を調整
  align_mem = (SceUID*)(((int)p + align - 1) & ~(align - 1));

  // mem_free出来るように返すメモリ位置の直前にSceUIDを書込む
  *(align_mem - 1) = *(p - 1);

  return align_mem;
}
*/

/*---------------------------------------------------------------------------
  メモリ解放
    *ptr: 確保したメモリへのポインタ
    return: エラーの場合は負の値を返す
---------------------------------------------------------------------------*/
s32 mem_free(void *ptr)
{
  if(ptr == NULL)
    return 0;

  return sceKernelFreePartitionMemory((SceUID)*((u32 *)ptr - 1));
}

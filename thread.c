// スレッド

// ヘッダー
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pspthreadman.h>
#include <pspsysmem_kernel.h>
#include <kubridge.h>
#include <pspmscm.h>
#include "thread.h"

#define IO_MEM_STICK_STATUS *((volatile int*)(0xBD200038))

#define SCE_INIT_BOOT_EF	0x50
#define PSP_GO					4
#define READY					4

// グローバル変数
static int first_thid[MAX_THREAD];
static int first_count;

static int current_thid[MAX_THREAD];
static int current_count = -1;


//static bool threadsState = false;// == ture : suspend / == false : resume

//static clock_t safelySuspendTime;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Waits until the memory stick/internal storage is ready									*/
/* Based on custom firmware Pro C2 source, thanks to Omega2058 at Wololo for the information*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
void waitMsready(){
	int ret, status, i;
	const char *drvname;

	status = 0;
	drvname = "mscmhc0:";

	if (kuKernelGetModel() == PSP_GO && kuKernelBootFrom() == SCE_INIT_BOOT_EF)
	{
		drvname = "mscmhcemu0:";
		while (1)
		{
			ret = sceIoDevctl(drvname, 0x02025801, 0, 0, &status, sizeof(status));
			
			if (ret < 0)
			{
				sceKernelDelayThread(1);
				continue;
			}
			
			if (status == READY)
			{
				break;
			}
				sceKernelDelayThread(1);
		}
	//For memory stick access the registry directly
	}else{
		if (MScmIsMediumInserted()){
			// wait MS (Thanks to hiroi, taba)
			for(i = 0; i < 100; i++){
				if((IO_MEM_STICK_STATUS & 0x2000) == 0){
					i = 0;
				}
				sceKernelDelayThread(1);
			}
		}
	}

	return;
}


/*
void safelySuspendThreadsInit()
{
	safelySuspendTime = sceKernelLibcClock();
}

int safelySuspendThreads( clock_t waitTime )
{
	if( ! threadsState ){
		if( (sceKernelLibcClock() - safelySuspendTime) >= waitTime ){
//			Suspend_Resume_Threads(SUSPEND_MODE);
			suspendThreads();
			return 1;
		}
		return -1;
	}
	
	return 0;
}

void suspendThreads()
{
	if( ! threadsState ){
		Suspend_Resume_Threads(SUSPEND_MODE);
		threadsState = true;
	}	
}

void resumeThreads()
{
	if( threadsState ){
		Suspend_Resume_Threads(RESUME_MODE);
		threadsState = false;
	}
}

bool Get_ThreadModeStatus()
{
	
	return threadsState;
}


int Get_ThreadStatus()
{
	SceKernelThreadInfo thinfo;
	int threads[MAX_THREAD];
	int threads_count;

	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, threads, MAX_THREAD, &threads_count);

	memset(&thinfo, 0, sizeof(SceKernelThreadInfo));
	thinfo.size = sizeof(SceKernelThreadInfo);
	sceKernelReferThreadStatus(threads[threads_count-1], &thinfo);
	if( thinfo.status & PSP_THREAD_SUSPEND )return 1;

	return 0;

}
*/

void Get_FirstThreads()
{
	// スレッド一覧を取得
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, first_thid, MAX_THREAD, &first_count);
}

/*
void Suspend_Resume_Threads(int mode)
{
	int i, n;
	SceUID my_thid;
	SceUID (*Thread_Func)(SceUID) = NULL;
	SceKernelThreadInfo thinfo;

	my_thid = sceKernelGetThreadId();
	Thread_Func = (mode == RESUME_MODE ? sceKernelResumeThread : sceKernelSuspendThread);

	// wait MS (Thanks to hiroi, taba)
	for(i = 0; i < 1000; i++){
		if((IO_MEM_STICK_STATUS & 0x2000) == 0){
			i = 0;
		}
		sceKernelDelayThread(1);
	}

	// スレッド一覧を取得

	if(mode == SUSPEND_MODE)
	{
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, current_thid, MAX_THREAD, &current_count);

		for(i = 0; i < current_count; i++)
		{

			memset(&thinfo, 0, sizeof(SceKernelThreadInfo));
			thinfo.size = sizeof(SceKernelThreadInfo);
			sceKernelReferThreadStatus(current_thid[i], &thinfo);
			if( thinfo.status & PSP_THREAD_SUSPEND || current_thid[i] == my_thid )
			{
				current_thid[i] = 0;
				continue;
			}

			for(n = 0; n < first_count; n++)
			{
				if(current_thid[i] == first_thid[n])
				{
					current_thid[i] = 0;
					n = first_count;
				}
			}
		}
	}
	
	//最終的な現在のスレッドリストにあるスレッドへ停止・再開の操作
	for( i = 0; i < current_count; i++ )
	{
		if( current_thid[i] )Thread_Func(current_thid[i]);
	}
	return;
}
*/

inline int threadCtrlIsSuspending()
{
	return (current_count < 0)?0:1;
}

int threadCtrlInit()
{
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, first_thid, MAX_THREAD, &first_count);
	return 0;
}

int threadCtrlSuspend()
{
	if( threadCtrlIsSuspending() ){
		return 1;
	}
	
	int i, n;
	SceUID this_thid;
	SceKernelThreadInfo thinfo;
	
	this_thid = sceKernelGetThreadId();
	
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, current_thid, MAX_THREAD, &current_count);
	
	for(i = 0; i < current_count; i++){
		memset(&thinfo, 0, sizeof(SceKernelThreadInfo));
		thinfo.size = sizeof(SceKernelThreadInfo);
		sceKernelReferThreadStatus(current_thid[i], &thinfo);
		
		if(thinfo.status & PSP_THREAD_SUSPEND || current_thid[i] == this_thid){
			current_thid[i] = -1;
			continue;
		}
		
		for(n = 0; n < first_count; n++){
			if(current_thid[i] == first_thid[n]){
				current_thid[i] = -1;
				break;
			}
		}
	}
	
	waitMsready();
	
	for(i = 0; i < current_count; i++){
		if( current_thid[i] >= 0 ){
			sceKernelSuspendThread(current_thid[i]);
		}
	}
	
	return 0;
}

int threadCtrlResume()
{
	if( !threadCtrlIsSuspending() ){
		return 1;
	}
	
	int i;
	for(i = 0; i < current_count; i++){
		if(current_thid[i] >= 0 )
			sceKernelResumeThread(current_thid[i]);
	}
	
	current_count = -1;
	
	return 0;
}

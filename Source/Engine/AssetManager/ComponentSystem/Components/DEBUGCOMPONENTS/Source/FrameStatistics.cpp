#include "AssetManager.pch.h" 
#include <conio.h>
#include <pdh.h>
#include <psapi.h>
#include <stdio.h>
#include <TCHAR.h> 
#include <Tools/ImGui/ImGui/imgui.h> 
#include <Tools/Utilities/Math.hpp>
#include <windows.h>
#include "../FrameStatistics.h"

#pragma comment(lib,"pdh.lib") 
static ULARGE_INTEGER lastCPU,lastSysCPU,lastUserCPU;
static int numProcessors;
static HANDLE self;

double App_CPU_Usage();

FrameStatistics::FrameStatistics(const SY::UUID anOwnerId) : Component(anOwnerId)
{
#ifdef  _WIN32   
	SYSTEM_INFO sysInfo;
	FILETIME ftime,fsys,fuser;

	GetSystemInfo(&sysInfo);
	numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&lastCPU,&ftime,sizeof(FILETIME));

	self = GetCurrentProcess();
	GetProcessTimes(self,&ftime,&ftime,&fsys,&fuser);
	memcpy(&lastSysCPU,&fsys,sizeof(FILETIME));
	memcpy(&lastUserCPU,&fuser,sizeof(FILETIME));
#endif
}

void FrameStatistics::Update()
{
	const float framerate = 1 / Timer::GetInstance().GetDeltaTime();

	if (myDataIndex > MAX_DATA_COUNT - 1)
	{
		myDataIndex = 0;
	}

	myAverageFPS[myDataIndex] = framerate;

	float sum = 0;
	sum = std::accumulate(myAverageFPS,myAverageFPS + MAX_DATA_COUNT,sum) / MAX_DATA_COUNT;

	ImGui::Begin("Frame statistics");
	ImGui::Text("FPS: (%f)\n",sum);

#ifdef  _WIN32  

	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(),(PROCESS_MEMORY_COUNTERS*)&pmc,sizeof(pmc));
	myAverageVirtualMemoryByApp[myDataIndex] = pmc.PrivateUsage / Mega;  // megabyte?
	sum = std::accumulate(myAverageVirtualMemoryByApp,myAverageVirtualMemoryByApp + MAX_DATA_COUNT,sum) / MAX_DATA_COUNT;
	ImGui::Text("Total Virtual used by app: (%f)\n",myAverageVirtualMemoryByApp);

	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	myAverageMemoryByApp[myDataIndex] = (float)physMemUsedByMe / Mega;
	sum = std::accumulate(myAverageMemoryByApp,myAverageMemoryByApp + MAX_DATA_COUNT,sum) / MAX_DATA_COUNT;
	ImGui::Text("Total physcial memory used by app: (%f)\n",sum);

	myAverageCPUByApp[myDataIndex] = (float)App_CPU_Usage();
	sum = std::accumulate(myAverageCPUByApp,myAverageCPUByApp + MAX_DATA_COUNT,sum) / MAX_DATA_COUNT;
	ImGui::Text("Total CPU usage by app: (%f)\n",sum);

#endif //  _WIN32
	ImGui::End();
	myDataIndex++;
}

double App_CPU_Usage()
{
	FILETIME ftime,fsys,fuser;
	ULARGE_INTEGER now,sys,user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now,&ftime,sizeof(FILETIME));

	GetProcessTimes(self,&ftime,&ftime,&fsys,&fuser);
	memcpy(&sys,&fsys,sizeof(FILETIME));
	memcpy(&user,&fuser,sizeof(FILETIME));
	percent = ((double)sys.QuadPart - (double)lastSysCPU.QuadPart) +
		(user.QuadPart - lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numProcessors;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;

	return percent * 100;
}
#include "../FrameStatistics.h"

#include <conio.h>
#include <numeric>
#include <pdh.h>
#include <psapi.h>
#include <stdio.h>
#include <TCHAR.h>
#include <windows.h>
#include <DirectX/DX12/Graphics/GPU.h>
#include <Tools/ImGui/imgui.h>
#include <Tools/Utilities/Math.hpp>
#include <Tools/Utilities/Game/Timer.h>

#pragma comment(lib, "pdh.lib")
static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int            numProcessors;
static HANDLE         self;

double App_CPU_Usage();

FrameStatistics::FrameStatistics() : EditorWindow()
{
#ifdef _WIN32
	SYSTEM_INFO sysInfo;
	FILETIME    ftime;
	FILETIME    fsys;
	FILETIME    fuser;

	GetSystemInfo(&sysInfo);
	numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&lastCPU, &ftime, sizeof(FILETIME));

	self = GetCurrentProcess();
	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
#endif
}

void FrameStatistics::RenderImGUi()
{
	if (myDataIndex > MAX_DATA_COUNT - 1)
	{
		myDataIndex = 0;
	}

	ImGui::Begin("Frame statistics", &m_KeepWindow);
	float sum = 0;
	sum = std::accumulate(myAverageFPS, myAverageFPS + MAX_DATA_COUNT, sum) / MAX_DATA_COUNT;
	ImGui::Text("FPS: (%f)\n", sum);
	ImGui::PlotLines("FPS", &myAverageFPS[0], MAX_DATA_COUNT, myDataIndex);

#ifdef _WIN32
	if (skipFrame < SKIPRATE)
	{
		skipFrame++;
	}
	else
	{
		// Update the data every SKIPRATE frames

		const float framerate = 1 / TimerInstance.getDeltaTime();
		myAverageFPS[myDataIndex] = framerate;

		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		myAverageVirtualMemoryByApp[myDataIndex] = pmc.PrivateUsage / Mega; // megabyte?

		const SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
		myAverageMemoryByApp[myDataIndex] = static_cast<float>(physMemUsedByMe) / Mega;

		myAverageCPUByApp[myDataIndex] = static_cast<float>(App_CPU_Usage());

		const auto mem = GPUInstance.m_GraphicsMemory->GetStatistics();
		m_committedMemory[myDataIndex] = static_cast<float>(mem.committedMemory);
		m_totalMemory[myDataIndex] = static_cast<float>(mem.totalMemory);
		m_totalPages[myDataIndex] = static_cast<float>(mem.totalPages);
		m_peakCommitedMemory[myDataIndex] = static_cast<float>(mem.peakCommitedMemory);
		m_peakTotalMemory[myDataIndex] = static_cast<float>(mem.peakTotalMemory);

		skipFrame = 0;
		myDataIndex++;
	}

	sum = std::accumulate(myAverageVirtualMemoryByApp, myAverageVirtualMemoryByApp + MAX_DATA_COUNT, sum) /
		MAX_DATA_COUNT;
	ImGui::Text("Total Virtual used by app: (%f)\n", sum);
	ImGui::PlotLines("Virtual memory", &myAverageVirtualMemoryByApp[0], MAX_DATA_COUNT, myDataIndex);

	sum = std::accumulate(myAverageMemoryByApp, myAverageMemoryByApp + MAX_DATA_COUNT, sum) / MAX_DATA_COUNT;
	ImGui::Text("Total physcial memory used by app: (%f)\n", sum);
	ImGui::PlotLines("Physical memory", &myAverageMemoryByApp[0], MAX_DATA_COUNT, myDataIndex);

	sum = std::accumulate(myAverageCPUByApp, myAverageCPUByApp + MAX_DATA_COUNT, sum) / MAX_DATA_COUNT;
	ImGui::Text("Total CPU usage by app: (%f)\n", sum);
	ImGui::PlotLines("CPU usage", &myAverageCPUByApp[0], MAX_DATA_COUNT, myDataIndex);

	sum = std::accumulate(m_committedMemory, m_committedMemory + MAX_DATA_COUNT, sum) / MAX_DATA_COUNT;
	ImGui::Text("GPU:	Total committed memory: (%f)\n", sum);
	ImGui::PlotLines("GPU:	Committed memory", &m_committedMemory[0], MAX_DATA_COUNT, myDataIndex);

	sum = std::accumulate(m_totalMemory, m_totalMemory + MAX_DATA_COUNT, sum) / MAX_DATA_COUNT;
	ImGui::Text("GPU:	Total memory: (%f)\n", sum);
	ImGui::PlotLines("GPU:	Total memory", &m_totalMemory[0], MAX_DATA_COUNT, myDataIndex);

	sum = std::accumulate(m_totalPages, m_totalPages + MAX_DATA_COUNT, sum) / MAX_DATA_COUNT;
	ImGui::Text("GPU:	Total pages: (%f)\n", sum);
	ImGui::PlotLines("GPU:	Total pages", &m_totalPages[0], MAX_DATA_COUNT, myDataIndex);

	sum = std::accumulate(m_peakCommitedMemory, m_peakCommitedMemory + MAX_DATA_COUNT, sum) / MAX_DATA_COUNT;
	ImGui::Text("GPU:	Peak committed memory: (%f)\n", sum);
	ImGui::PlotLines("GPU:	Peak committed memory", &m_peakCommitedMemory[0], MAX_DATA_COUNT, myDataIndex);

	sum = std::accumulate(m_peakTotalMemory, m_peakTotalMemory + MAX_DATA_COUNT, sum) / MAX_DATA_COUNT;
	ImGui::Text("GPU:	Peak total memory: (%f)\n", sum);
	ImGui::PlotLines("GPU:	Peak total memory", &m_peakTotalMemory[0], MAX_DATA_COUNT, myDataIndex);

	sum = std::accumulate(m_peakTotalPages, m_peakTotalPages + MAX_DATA_COUNT, sum) / MAX_DATA_COUNT;
	ImGui::Text("GPU:	Peak total pages: (%f)\n", sum);
	ImGui::PlotLines("GPU:	Peak total pages", &m_peakTotalPages[0], MAX_DATA_COUNT, myDataIndex);

#endif //  _WIN32
	ImGui::End();
}

double App_CPU_Usage()
{
	FILETIME       ftime, fsys, fuser;
	ULARGE_INTEGER now,   sys,  user;
	double         percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (static_cast<double>(sys.QuadPart) - static_cast<double>(lastSysCPU.QuadPart)) + (user.QuadPart -
		lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numProcessors;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;

	return percent * 100;
}

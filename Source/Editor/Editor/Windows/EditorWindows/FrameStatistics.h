#pragma once
#include "EditorWindow.h"

class FrameStatistics : public EditorWindow
{
  public:
    FrameStatistics();
    void RenderImGUi() override;
    ~FrameStatistics() = default;

  private:
	void APP_VirtualPhysicalMemory( );
#define MAX_DATA_COUNT 100
#define SKIPRATE 1
    int skipFrame = 0;
    int myDataIndex = 0;
    float myAverageFPS[MAX_DATA_COUNT] = {0.0f};
    float myAverageVirtualMemoryByApp[MAX_DATA_COUNT] = {0.0f};
    float myAverageMemoryByApp[MAX_DATA_COUNT] = {0.0f};
    float myAverageCPU[MAX_DATA_COUNT] = {0.0f};
    float myAverageCPUByApp[MAX_DATA_COUNT] = {0.0f};

    float m_committedMemory[MAX_DATA_COUNT];    // Bytes of memory currently
                                                // committed/in-flight
    float m_totalMemory[MAX_DATA_COUNT];        // Total bytes of memory used by the
                                                // allocators
    float m_totalPages[MAX_DATA_COUNT];         // Total page count
    float m_peakCommitedMemory[MAX_DATA_COUNT]; // Peak commited memory value
                                                // since last reset
    float m_peakTotalMemory[MAX_DATA_COUNT];    // Peak total bytes
    float m_peakTotalPages[MAX_DATA_COUNT];     // Peak total page count
};

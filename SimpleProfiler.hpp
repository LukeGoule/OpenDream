#pragma once

#include <vector>
#include <chrono>

#define MAX_FRAME_HISTORY 150

class SimpleProfiler
{
	float m_vecFPSHistory[MAX_FRAME_HISTORY];
	std::chrono::steady_clock::time_point m_AppStart, m_FrameStart, m_FrameEnd;
	std::size_t m_iCurrentFrame = 0;
	long long m_iAverageFPS = 0, m_iCurrentFPS, m_iMaxFPS = INT_MIN, m_iMinFPS = INT_MAX;

public:

	// constr
	SimpleProfiler();
	
	// destr
	~SimpleProfiler();

	
	// Call this juse before the mainloop
	void StartApp();

	// Resets all the member vars
	void Reset();

	
	// Call this before rendering / processing begins
	void FrameStart();

	// Call this after all rendering has completed
	void FrameEnd();


	// Returns the current FPS as calculated in FrameEnd
	unsigned long long FPS();

	// Returns a 20-frame average of the FPS
	unsigned long long FPSAverage();

	// Returns the maximum seen average FPS
	unsigned long long FPSMax();

	// Returns the minimum seen average FPS
	unsigned long long FPSMin();

	// Does what it says
	float SecondsSinceStart();

	//  History of frames. Over-writes itself once it hits MAX_FRAME_HISTORY frames.
	float* History();
};
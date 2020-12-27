#include "SimpleProfiler.hpp"

SimpleProfiler::SimpleProfiler()
{
	Reset();
}

SimpleProfiler::~SimpleProfiler()
{

}

void SimpleProfiler::StartApp()
{
	m_AppStart = std::chrono::high_resolution_clock::now();
	m_iCurrentFrame = 0;
	m_iAverageFPS = 0;

	Reset();
}

void SimpleProfiler::Reset()
{
	for (int i = 0; i < MAX_FRAME_HISTORY; i++)
	{
		m_vecFPSHistory[i] = 0.0f;
	}
}

void SimpleProfiler::FrameStart()
{
	m_FrameStart = std::chrono::high_resolution_clock::now();
}

void SimpleProfiler::FrameEnd()
{
	m_FrameEnd = std::chrono::high_resolution_clock::now();

    auto FrameTimeMS = std::chrono::duration_cast<std::chrono::nanoseconds>(m_FrameEnd - m_FrameStart).count();

    m_iCurrentFPS = (1 * 1000 * 1000 * 1000) / FrameTimeMS;

    m_vecFPSHistory[m_iCurrentFrame] = m_iCurrentFPS;

    if (m_iCurrentFrame == MAX_FRAME_HISTORY - 1)
    {
        m_iCurrentFrame = 0;
    }

    if (m_iCurrentFrame % 20 == 0 && m_iCurrentFrame != 0)
    {
        long long n = 0;
        for (int i = 20; i > 0; i--)
        {
            n += m_vecFPSHistory[m_iCurrentFrame - i];
        }
        m_iAverageFPS = n / 20;

        if (m_iAverageFPS > m_iMaxFPS)
        {
            m_iMaxFPS = m_iAverageFPS;
        }

        if (m_iAverageFPS < m_iMinFPS)
        {
            m_iMinFPS = m_iAverageFPS;
        }
    }

    m_iCurrentFrame++;
}

unsigned long long SimpleProfiler::FPS()
{
    return m_iCurrentFPS;
}

unsigned long long SimpleProfiler::FPSAverage()
{
    return m_iAverageFPS;
}

unsigned long long SimpleProfiler::FPSMax()
{
    return m_iMaxFPS;
}

unsigned long long SimpleProfiler::FPSMin()
{
    return m_iMinFPS;
}

float SimpleProfiler::SecondsSinceStart()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_FrameEnd - m_AppStart).count() / 1000.f;
}

float* SimpleProfiler::History()
{
    return m_vecFPSHistory;
}
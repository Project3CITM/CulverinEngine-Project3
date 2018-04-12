// ----------------------------------------------------
// j1PerfTimer.cpp
// Slow timer with microsecond precision
// ----------------------------------------------------

#include "PerfTimer.h"
#include "SDL\include\SDL_timer.h"


uint64 PerfTimer::frequency = 0;

// ---------------------------------------------
PerfTimer::PerfTimer()
{
	if (frequency == 0)
		frequency = SDL_GetPerformanceFrequency();

	Start();
}

// ---------------------------------------------
void PerfTimer::Start()
{
	started_at = Clock::now();
}

// ---------------------------------------------
double PerfTimer::ReadMs() const
{

	std::chrono::duration<double> diff = Clock::now() - started_at;

	return diff.count()*1000;
}

// ---------------------------------------------
uint64 PerfTimer::ReadTicks() const
{
	return 0;// SDL_GetTicks() - started_at;
}



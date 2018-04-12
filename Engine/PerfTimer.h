#ifndef __j1PERFTIMER_H__
#define __j1PERFTIMER_H__

#include "Globals.h"
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

class PerfTimer
{
public:

	// Constructor
	PerfTimer();

	void Start();
	double ReadMs() const;
	uint64 ReadTicks() const;

private:
	std::chrono::steady_clock::time_point	started_at;
	static uint64 frequency;
};

#endif //__j1PERFTIMER_H__
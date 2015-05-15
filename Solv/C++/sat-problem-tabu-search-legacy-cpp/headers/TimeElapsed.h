#pragma once
#include <time.h>

class TimeElapsed
{
public:
	TimeElapsed(void);
	double GetTimeElapsed(clock_t clock1, clock_t clock2);
	~TimeElapsed(void);
};
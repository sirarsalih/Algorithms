#include "TimeElapsed.h"

TimeElapsed::TimeElapsed(void)
{
}

double TimeElapsed::GetTimeElapsed(clock_t clock1, clock_t clock2)
{
	double diffTicks = clock1 - clock2;
	double diffMS = (diffTicks * 1000)/CLOCKS_PER_SEC;
	
	return diffMS;
}

TimeElapsed::~TimeElapsed(void)
{
}

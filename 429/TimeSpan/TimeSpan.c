#include "TimeSpan.h"

uint32_t GetTimeSpan(uint32_t lastTickTime, uint32_t currentTickTime)
{
	if(lastTickTime<currentTickTime)return currentTickTime-lastTickTime;
	else return 0xFFFFFFFF-lastTickTime + currentTickTime +1;
}



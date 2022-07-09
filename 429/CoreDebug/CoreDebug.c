#include "CoreDebug.h"

unsigned char IsDebuggerConnected(void)
{
 	return (CoreDebugPtr->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) ? 0x01 : 0x00;
}


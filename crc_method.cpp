#include "crc_method.h"
#include "main.h"


DWORD (__stdcall *Search)(LPVOID _threadnum) = 0;

void SetupSearch(void)
{
	if (loops_enabled)
	{
		if (loop_unrolling)
		{
			Search = &SearchLoopUnroll;
		}
		else
		{
			Search = &SearchLoopNoUnroll;
		}
	}
	else Search = &SearchRecurse;
}
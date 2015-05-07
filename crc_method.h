#ifndef CRC_METHOD_H
#define CRC_METHOD_H
#include "main.h"
#include <Windows.h>

extern DWORD __stdcall SearchLoopNoUnroll(LPVOID _threadnum);
extern DWORD __stdcall SearchLoopUnroll(LPVOID _threadnum);
extern DWORD __stdcall SearchRecurse(LPVOID _threadnum);

extern DWORD (__stdcall *Search)(LPVOID _threadnum);

extern void SetupSearch(void);

#endif
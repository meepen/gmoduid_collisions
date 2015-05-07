#include "crc_method.h"
#include "main.h"
#include <atomic>

#define MAX_CHARS (8)

std::atomic_llong amtdone;

#define DOIT2(x, two) \
	if(!two) \
			{ \
		CRC32(&nextinline, x, 1, &cur); \
		CRC32(&nextinline, "_gm", 3); \
		CRC32_Final(&nextinline); \
		if(nextinline == want) \
			printf("Found collision: STEAM_0:%i:%s%s\n", begin, temp, x); \
		nextinline = cur; \
		} \
		else\
		Recurse(begin, want, nextinline, amtsofar + 2, x);


#define DOIT1(x) \
	DOIT2(x "0", true); \
	DOIT2(x "1", true); \
	DOIT2(x "2", true); \
	DOIT2(x "3", true); \
	DOIT2(x "4", true); \
	DOIT2(x "5", true); \
	DOIT2(x "6", true); \
	DOIT2(x "7", true); \
	DOIT2(x "8", true); \
	DOIT2(x "9", true); 

#define DOIT() \
	if(amtsofar != MAX_CHARS) \
		{ \
		DOIT2("0", false); \
		DOIT2("1", false); \
		DOIT2("2", false); \
		DOIT2("3", false); \
		DOIT2("4", false); \
		DOIT2("5", false); \
		DOIT2("6", false); \
		DOIT2("7", false); \
		DOIT2("8", false); \
		DOIT2("9", false); \
		if(amtsofar != -2) \
			{ DOIT1("0"); } \
		DOIT1("9"); \
		DOIT1("1"); \
		DOIT1("2"); \
		DOIT1("3"); \
		DOIT1("4"); \
		DOIT1("5"); \
		DOIT1("6"); \
		DOIT1("7"); \
		DOIT1("8"); \
		} 

__declspec(thread) static char temp[11] = { 0 };

void Recurse(char begin, CRC32_t want, CRC32_t cur, int amtsofar = -2, char *next = 0/* 2 chars at a time */)
{
	if (next)
	{
		temp[amtsofar] = *(next);
		temp[amtsofar + 1] = *(next + 1);
		temp[amtsofar + 2] = 0;
		CRC32(&cur, next, 2);
		CRC32_t t;
		CRC32(&t, "_gm", 3, &cur);
		CRC32_Final(&t);
		if (t == want)
		{
			printf("Found collision: STEAM_0:%i:%s\n", begin, temp);
		}
	}

	CRC32_t nextinline = cur;

	DOIT();

	if (next)
		temp[amtsofar] = 0;

	amtdone += 11;
}

DWORD __stdcall SearchRecurse(LPVOID __threadid)
{
	int threadid = (int)__threadid;
	printf("Beginning search for thread id %i\n", threadid);

	Recurse(threadid - 1, crc, begin_crc[threadid - 1]);
	return 0;
}
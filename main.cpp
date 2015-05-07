#define _CRT_SECURE_NO_WARNINGS


// if defined, it will use a recursive method
//#define NO_LOOP

// unroll loops 2 times
// comment if it's slower
// reason: it's only faster on some cpus
// if NO_LOOP is defined this does nothing!
//#define LOOP_UNROLLING 

#define MAX_THREADS (threads)
#define MAX_IDS (1000000000)
#define increase_wait (10000000)
#define loop_size (100)
#define increase_wait_rounded ((increase_wait / loop_size) * loop_size)

#ifndef NO_LOOP

#ifdef LOOP_UNROLLING


// only unroll 1x
// unrolling more = long compile time & slower speeds
#define LOOPME(n) \
	for (int i = 0; i < 10; i++)  \
	{ \
	\
		CalculateUniqueID(&crc0, num2, 6, 0, &temp0_); \
		CalculateUniqueID(&crc1, num2, 6, 0, &temp1_); \
		if (crc1 == crc) \
		{ \
			printf("Found id: STEAM_0:1:%I64d\n", start + i + n); \
		} \
		if (crc0 == crc) \
		{ \
			printf("Found id: STEAM_0:0:%I64d\n", start + i + n); \
		} \
		change++; \
	} \
	change = '0';


#define LOOP()\
	LOOPME(00); \
	change2 = '1';\
	LOOPME(10);\
	change2 = '2';\
	LOOPME(20);\
	change2 = '3';\
	LOOPME(30);\
	change2 = '4';\
	LOOPME(40);\
	change2 = '5';\
	LOOPME(50);\
	change2 = '6';\
	LOOPME(60);\
	change2 = '7';\
	LOOPME(70);\
	change2 = '8';\
	LOOPME(80);\
	change2 = '9';\
	LOOPME(90);

#else // LOOP_UNROLLING

#define LOOP() \
	for(int x = 0; x < 10; x++) \
	{ \
		for (int i = 0; i < 10; i++)  \
		{ \
		\
			CalculateUniqueID(&crc0, num2, 6, 0, &temp0_); \
			CalculateUniqueID(&crc1, num2, 6, 0, &temp1_); \
			if (crc1 == crc) \
			{ \
				printf("Found id: STEAM_0:1:%I64d\n", start + i + x * 10 + i); \
			} \
			if (crc0 == crc) \
			{ \
				printf("Found id: STEAM_0:0:%I64d\n", start + i + x * 10 + i); \
			} \
			change++; \
		} \
		change = '0'; \
		change2++; \
	} \


#endif // LOOP_UNROLL
#define FIND() LOOP()
#endif // NO_LOOP

#include <string>
#include <time.h>
#include <nmmintrin.h>
#include <Windows.h>
#include <conio.h>
#include <atomic>
#include <stdlib.h>
typedef uint32_t CRC32_t;

unsigned long threads = 2;

#define TRY_NEW

extern void CRC32_ProcessBuffer(CRC32_t *pulCRC, const void *pBuffer, int nBuffer);
extern uint32_t crc32_16bytes_prefetch(const void* data, size_t length, uint32_t previousCrc32 = 0, size_t prefetchAhead = 256);

inline void CRC32(CRC32_t *crc, const void *buffer, int len, CRC32_t *before = 0)
{
#ifdef TRY_NEW
	*crc = crc32_16bytes_prefetch(buffer, len, before ? *before : *crc);
#else
	CRC32_ProcessBuffer(crc, buffer, len);
#endif
}
extern int format_time(char *output, clock_t time);

#pragma comment(linker, "/ENTRY:main")

#define CRC32_INIT_VALUE 0xFFFFFFFFUL
#define CRC32_XOR_VALUE  0xFFFFFFFFUL

inline void CRC32_Init(CRC32_t *pulCRC)
{
	*pulCRC = CRC32_INIT_VALUE;
}

inline void CRC32_Final(CRC32_t *pulCRC)
{
	*pulCRC ^= CRC32_XOR_VALUE;
}


CRC32_t begin_crc[2];

inline void CalculateUniqueID(CRC32_t *crc, char *id, int len, char which, CRC32_t *start = 0, bool finish = true)
{
	if (start)
		*crc = *start;
	else
		*crc = begin_crc[which];

	CRC32(crc, id, len);
	if (finish)
		CRC32_Final(crc);
}

clock_t start_clock = 0;

std::atomic_llong *done = 0;
std::atomic_llong max;
std::atomic_llong completed;


DWORD __stdcall Speedtest(LPVOID)
{
	char title[512];
	while (completed < max)
	{
#ifdef NO_LOOP
		extern std::atomic_llong amtdone;
		completed = amtdone;
#else
		completed = 0;
		for (unsigned int i = 0; i < MAX_THREADS; i++)
			completed += done[i];
		completed = completed * 2;
#endif
		auto dif = (clock() - start_clock);
		sprintf(title, "%.2f per second (%I64d searched) %.2f%%", completed / (double(dif) / double(CLOCKS_PER_SEC)), completed, double(completed) / double(max) * 100);
		SetConsoleTitleA(title);
		Sleep(500);
	}
	return 0;
}

CRC32_t crc = 0;
HANDLE *threadarray;


#ifdef NO_LOOP
#define MAX_CHARS (6)

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

DWORD __stdcall Search(LPVOID __threadid)
{
	int threadid = (int)__threadid;
	printf("Beginning search for thread id %i\n", threadid);

	Recurse(threadid - 1, crc, begin_crc[threadid - 1]);
	return 0;
}

#else // NO_LOOP
DWORD __stdcall Search(LPVOID _threadnum)
{
	int threadnum = (int)_threadnum;
	long long start = (threadnum - 1) * (MAX_IDS / MAX_THREADS);
	long long end = (threadnum) * (MAX_IDS / MAX_THREADS);
	max += (end - start) * 2;
	std::atomic_llong &increase = done[threadnum - 1];
	char num[64];
	printf("Thread %i: %I64d - %I64d\n", threadnum, start, end);
	SuspendThread(threadarray[threadnum - 1]);

	CRC32_t tcrc;

	long a = increase_wait;
	
	while ((start % loop_size) != 0 || start < (loop_size - 1))
	{
		int len = wsprintfA(num, "%I64d_gm", start); 
		CalculateUniqueID(&tcrc, num, len, 0);
		if (crc == tcrc)
		{
			printf("Found id: STEAM_0:0:%I64d\n", start);
		}
		CalculateUniqueID(&tcrc, num, len, 1);
		if (crc == tcrc)
		{
			printf("Found id: STEAM_0:1:%I64d\n", start);
		}
		if ((--a) == 0)
		{
			a = increase_wait;
			increase += increase_wait;
		}
		start++;
	}
	increase += increase_wait - a;
	a = increase_wait_rounded;
	CRC32_t temp0_;
	CRC32_t temp1_;
	CRC32_t crc0, crc1;
	while ((start + loop_size) < end)
	{
		int len = wsprintfA(num, "%I64d_gm", start);
		char *num2 = num;

		len -= 5;

		num2 += len;
		char &change = num2[1];
		char &change2 = num2[0];

		CalculateUniqueID(&temp0_, num, len, 0, 0, false);
		CalculateUniqueID(&temp1_, num, len, 1, 0, false);

		FIND();

		a -= loop_size;

		if (a == 0)
		{
			a = increase_wait_rounded;
			increase += increase_wait_rounded;
		}

		start += loop_size;
	}
	increase += increase_wait_rounded - a;

	while (++start <= end)
	{
		int len = wsprintfA(num, "%I64d_gm", start);
		CalculateUniqueID(&tcrc, num, len, 0);
		if (crc == tcrc)
		{
			printf("Found id: STEAM_0:0:%I64d\n", start);
		}
		CalculateUniqueID(&tcrc, num, len, 1);
		if (crc == tcrc)
		{
			printf("Found id: STEAM_0:1:%I64d\n", start);
		}
		increase++;
	}

	printf("Done with thread %i\n", threadnum);
	return 0;
}
#endif

int __stdcall main(void)
{
	max = 0;
	completed = 0;
	const char *cmdline = GetCommandLineA();

	CRC32_Init(&begin_crc[0]);
	CRC32(&begin_crc[0], "gm_STEAM_0:0:", 13);

	CRC32_Init(&begin_crc[1]);
	CRC32(&begin_crc[1], "gm_STEAM_0:1:", 13);

	char input[64];


	if (strstr(cmdline, "-sid"))
	{
		if (sscanf(strstr(cmdline, "-sid"), "-sid \"%s\"", input) != 1)
		{
			printf("Input SteamID: ");
			scanf("%s", input);
		}
		else
			input[strlen(input) - 1] = 0;
	}
	else
	{
		printf("Input SteamID: ");
		scanf("%s", input);
	}

	if (strstr(cmdline, "-threads"))
		sscanf(strstr(cmdline, "-threads"), "-threads %i", &threads);

	done = new std::atomic_llong[threads];
	for (unsigned long i = 0; i < threads; i++)
		done[i] = 0;

	CRC32_Init(&crc);
	CRC32(&crc, "gm_", 3);
	CRC32(&crc, input, strlen(input));
	CRC32(&crc, "_gm", 3);
	CRC32_Final(&crc);

	printf("SteamID: %s\nYour crc: 0x%08X\n", input, crc);

#ifndef NO_LOOP
	threadarray = new HANDLE[ MAX_THREADS ];
	for (unsigned int i = 0; i < MAX_THREADS; i++)
		threadarray[i] = CreateThread(0, 0, &Search, (LPVOID)(i + 1), 0, 0);

	Sleep(1000);
	printf("Max: %I64d\n\n\n", max);

	start_clock = clock();
	for (unsigned int i = 0; i < MAX_THREADS; i++)
		ResumeThread(threadarray[i]);
	CreateThread(0, 0, &Speedtest, 0, 0, 0);
#else
	max = MAX_IDS * 2;
	CreateThread(0, 0, &Speedtest, 0, 0, 0);

	amtdone = 0;
	start_clock = clock();

	CreateThread(0, 0, &Search, (LPVOID)1, 0, 0);
	CreateThread(0, 0, &Search, (LPVOID)2, 0, 0);

#endif


	while (completed < max) Sleep(1000);

	char timestr[64];
	format_time(timestr, clock() - start_clock);
	printf("Done! (%s)\n", timestr);

	_getch(); // pause

	return 0;
}


#define _CRT_SECURE_NO_WARNINGS

#define MAX_THREADS (threads)
#define MAX_IDS (10000000000)
#define increase_wait (10000000)
#define loop_size (1000)
#define increase_wait_rounded ((increase_wait / loop_size) * loop_size)


#define LOOPME(n) \
		for (int x = 0; x < 10; x++) \
		{ \
			for (int i = 0; i < 10; i++)  \
			{ \
				\
					CalculateUniqueID(&crc0, num2, 6, 0, &temp0_); \
					CalculateUniqueID(&crc1, num2, 6, 0, &temp1_); \
					if (crc1 == crc) \
					{ \
					printf("Found id: STEAM_0:1:%I64d\n", start + i + x * 10 + n); \
					} \
					if (crc0 == crc) \
					{ \
					printf("Found id: STEAM_0:0:%I64d\n", start + i + x * 10 + n); \
					} \
					change++; \
			} \
			change2++; \
			change = '0'; \
		}

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

inline void CRC32(CRC32_t *crc, const void *buffer, int len)
{
#ifdef TRY_NEW
	*crc = crc32_16bytes_prefetch(buffer, len, *crc);
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
long long max = 0;
long long completed = 0;


DWORD __stdcall Speedtest(LPVOID)
{
	char title[512];
	while (completed != max)
	{
		completed = 0;
		for (unsigned int i = 0; i < MAX_THREADS; i++)
			completed += done[i];
		completed = completed * 2;
		auto dif = (clock() - start_clock);
		sprintf(title, "%.2f per second (%I64d searched) %.2f%%", completed / (double(dif) / double(CLOCKS_PER_SEC)), completed, double(completed) / double(max) * 100);
		SetConsoleTitleA(title);
		Sleep(500);
	}
	return 0;
}

CRC32_t crc = 0;
HANDLE *threadarray;

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

		len -= 6;

		num2 += len;
		char &change = num2[2];
		char &change2 = num2[1];
		char &change3 = num2[0];

		CalculateUniqueID(&temp0_, num, len, 0, 0, false);
		CalculateUniqueID(&temp1_, num, len, 1, 0, false);

		LOOPME(000);
		change3 = '1';
		LOOPME(100);
		change3 = '2';
		LOOPME(200);
		change3 = '3';
		LOOPME(300);
		change3 = '4';
		LOOPME(400);
		change3 = '5';
		LOOPME(500);
		change3 = '6';
		LOOPME(600);
		change3 = '7';
		LOOPME(700);
		change3 = '8';
		LOOPME(800);
		change3 = '9';
		LOOPME(900);
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
extern uint32_t **Crc32Lookup;

int __stdcall main(void)
{
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


	threadarray = new HANDLE[ MAX_THREADS ];
	for (unsigned int i = 0; i < MAX_THREADS; i++)
		threadarray[i] = CreateThread(0, 0, &Search, (LPVOID)(i + 1), 0, 0);

	Sleep(1000);
	printf("Max: %I64d\n\n\n", max);

	start_clock = clock();
	for (unsigned int i = 0; i < MAX_THREADS; i++)
		ResumeThread(threadarray[i]);

	CreateThread(0, 0, &Speedtest, 0, 0, 0);

	while (completed != max) Sleep(1000);

	char timestr[64];
	format_time(timestr, clock() - start_clock);
	printf("Done! (%s)\n", timestr);

	_getch(); // pause

	return 0;
}


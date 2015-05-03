#define _CRT_SECURE_NO_WARNINGS

#define MAX_THREADS (threads)
#define MAX_IDS (1000000000)
#define increase_wait (1000000)
#define increase_wait_rounded ((increase_wait / 10) * 10)

typedef unsigned long CRC32_t;

#include <string>
#include <time.h>
#include <nmmintrin.h>
#include <Windows.h>
#include <conio.h>
#include <atomic>

unsigned long threads = 2;

extern void CRC32_ProcessBuffer(CRC32_t *pulCRC, const void *pBuffer, int nBuffer);
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

	CRC32_ProcessBuffer(crc, id, len);
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

DWORD __stdcall Search(LPVOID _threadnum)
{
	int threadnum = (int)_threadnum;
	long long start = (threadnum - 1) * (MAX_IDS / MAX_THREADS);
	long long end = (threadnum) * (MAX_IDS / MAX_THREADS);
	max += (end - start) * 2;
	std::atomic_llong &increase = done[threadnum - 1];
	char num[64];
	printf("Thread %i: %I64d - %I64d\n", threadnum, start, end);

	CRC32_t tcrc;

	long a = increase_wait;
	
	while ((start % 10) != 0)
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
	while ((start + 10) < end)
	{
		int len = wsprintfA(num, "%I64d_gm", start);
		char *num2 = num;
		num2 += len - 4;
		char &change = num[len - 4];

		len -= 4;
		if (len < 0)
			len = 0;
		CalculateUniqueID(&temp0_, num, len, 0, 0, false);
		CalculateUniqueID(&temp1_, num, len, 1, 0, false);

		for (int i = 0; i < 10; i++)
		{
			CalculateUniqueID(&crc0, num2, 4, 0, &temp0_);
			CalculateUniqueID(&crc1, num2, 4, 0, &temp1_);
			if (crc1 == crc)
			{
				printf("Found id: STEAM_0:1:%I64d\n", start + i);
			}
			if (crc0 == crc)
			{
				printf("Found id: STEAM_0:0:%I64d\n", start + i);
			}
			change++;
		}
		a -= 10;

		if (a == 0)
		{
			a = increase_wait_rounded;
			increase += increase_wait_rounded;
		}

		start += 10;
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

int __stdcall main(void)
{
	const char *cmdline = GetCommandLineA();



	CRC32_Init(&begin_crc[0]);
	CRC32_ProcessBuffer(&begin_crc[0], "gm_STEAM_0:0:", 13);

	CRC32_Init(&begin_crc[1]);
	CRC32_ProcessBuffer(&begin_crc[1], "gm_STEAM_0:1:", 13);

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
	CRC32_ProcessBuffer(&crc, "gm_", 3);
	CRC32_ProcessBuffer(&crc, input, strlen(input));
	CRC32_ProcessBuffer(&crc, "_gm", 3);
	CRC32_Final(&crc);

	printf("SteamID: %s\nYour crc: 0x%08X\n", input, crc);


	start_clock = clock();
	for (unsigned int i = 0; i < MAX_THREADS; i++)
		CreateThread(0, 0, &Search, (LPVOID)(i + 1), 0, 0);

	Sleep(1000);
	printf("Max: %I64d\n\n\n", max);
	CreateThread(0, 0, &Speedtest, 0, 0, 0);

	while (completed != max) Sleep(1000);

	char timestr[64];
	format_time(timestr, clock() - start_clock);
	printf("Done! (%s)\n", timestr);

	_getch(); // pause

	return 0;
}


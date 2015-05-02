#define _CRT_SECURE_NO_WARNINGS

#define MAX_THREADS (threads)
#define MAX_IDS (1000000000)

typedef unsigned long CRC32_t;

#include <string>
#include <time.h>
#include <nmmintrin.h>
#include <Windows.h>
#include <conio.h>

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

inline void CalculateUniqueID(CRC32_t *crc, char *id, int len, char which)
{
	char temp[64];
	*crc = begin_crc[which];

	CRC32_ProcessBuffer(crc, temp, len);
	CRC32_Final(crc);
}

clock_t start_clock = 0;

long long *done = 0;
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
	long long &increase = done[threadnum - 1];
	char num[64];
	printf("Thread %i: %I64d - %I64d\n", threadnum, start, end);

	CRC32_t tcrc;
	
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

	done = new long long[threads];
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

	getch(); // pause

	return 0;
}


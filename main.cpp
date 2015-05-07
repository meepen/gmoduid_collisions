#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <time.h>
#include <nmmintrin.h>
#include <Windows.h>
#include <conio.h>
#include <atomic>
#include "main.h"
#include "crc_method.h"


// if defined, it will use a recursive method
#define NO_LOOP

// unroll loops 2 times
// comment if it's slower
// reason: it's only faster on some cpus
// if NO_LOOP is defined this does nothing!
//#define LOOP_UNROLLING 


#ifdef NO_LOOP
const bool loops_enabled = false;
const bool loop_unrolling = false;
#else // NO_LOOP

const bool loops_enabled = true;

#ifdef LOOP_UNROLLING

const bool loop_unrolling = true;

#else // LOOP_UNROLLING

const bool loop_unrolling = false;

#endif // LOOP_UNROLLING
#endif // NO_LOOP

unsigned long threads = 2;


extern int format_time(char *output, clock_t time);

#pragma comment(linker, "/ENTRY:main")


clock_t start_clock = 0;



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
HANDLE *threadarray = 0;
std::atomic_llong *done = 0;



CRC32_t begin_crc[2];
std::atomic_llong max, completed;

int __stdcall main(void)
{
	SetupSearch();
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
		threadarray[i] = CreateThread(0, 0, Search, (LPVOID)(i + 1), 0, 0);

	Sleep(1000);
	printf("Max: %I64d\n\n\n", max);

	start_clock = clock();
	for (unsigned int i = 0; i < MAX_THREADS; i++)
		ResumeThread(threadarray[i]);
	CreateThread(0, 0, &Speedtest, 0, 0, 0);
#else
	max = MAX_IDS * 2;
	CreateThread(0, 0, &Speedtest, 0, 0, 0);

	extern std::atomic_llong amtdone;
	amtdone = 0;
	start_clock = clock();

	CreateThread(0, 0, Search, (LPVOID)1, 0, 0);
	CreateThread(0, 0, Search, (LPVOID)2, 0, 0);

#endif


	while (completed < max) Sleep(1000);

	char timestr[64];
	format_time(timestr, clock() - start_clock);
	printf("Done! (%s)\n", timestr);

	_getch(); // pause

	return 0;
}


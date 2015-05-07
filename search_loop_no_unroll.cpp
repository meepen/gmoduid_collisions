#include "main.h"
#include "crc_method.h"


#define FIND() \
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
			} 



DWORD __stdcall SearchLoopNoUnroll(LPVOID _threadnum)
{
	int threadnum = (int)_threadnum;
	long long start = (threadnum - 1) * (MAX_IDS / MAX_THREADS);
	long long end = (threadnum)* (MAX_IDS / MAX_THREADS);
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
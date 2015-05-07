#ifndef MAIN_H
#define MAIN_H

#pragma warning(disable : 4244)

#include <stdint.h>
#include <atomic>
#include <Windows.h>

#define MAX_THREADS (threads)

extern unsigned long threads;

typedef uint32_t CRC32_t;

extern void CRC32_ProcessBuffer(CRC32_t *pulCRC, const void *pBuffer, int nBuffer);
extern uint32_t crc32_16bytes_prefetch(const void* data, size_t length, uint32_t previousCrc32 = 0, size_t prefetchAhead = 256);

inline void CRC32(CRC32_t *crc, const void *buffer, int len, CRC32_t *before = 0)
{
	*crc = crc32_16bytes_prefetch(buffer, len, before ? *before : *crc);
}

extern CRC32_t crc;
extern HANDLE *threadarray;
extern std::atomic_llong *done;
extern std::atomic_llong max;
extern std::atomic_llong completed;

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

extern CRC32_t begin_crc[2];

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


extern const bool loops_enabled;
extern const bool loop_unrolling;

const long long MAX_IDS = 100000000000;
const long long increase_wait = 10000000;
const long long loop_size = 100;
const long long increase_wait_rounded = (increase_wait / loop_size) * loop_size;

#endif
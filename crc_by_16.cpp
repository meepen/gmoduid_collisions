#include <stdlib.h>

// define endianess and some integer data types
#ifdef _MSC_VER
typedef unsigned __int8  uint8_t;
typedef unsigned __int32 uint32_t;
typedef   signed __int32  int32_t;

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __BYTE_ORDER    __LITTLE_ENDIAN

#include <xmmintrin.h>
#define PREFETCH(location) _mm_prefetch(location, _MM_HINT_T0)
#else
// uint8_t, uint32_t, in32_t
#include <stdint.h>
// defines __BYTE_ORDER as __LITTLE_ENDIAN or __BIG_ENDIAN
#include <endian.h>

#ifdef __GNUC__
#define PREFETCH(location) __builtin_prefetch(location)
#else
#define PREFETCH(location) ;
#endif
#endif


/// zlib's CRC32 polynomial
const uint32_t Polynomial = 0xEDB88320;

/// swap endianess
static inline uint32_t swap(uint32_t x)
{
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_bswap32(x);
#else
	return (x >> 24) |
		((x >> 8) & 0x0000FF00) |
		((x << 8) & 0x00FF0000) |
		(x << 24);
#endif
}


/// forward declaration, table is at the end of this file
extern uint32_t Crc32Lookup[16][256]; // extern is needed to keep compiler happey


/// compute CRC32 (bitwise algorithm)
uint32_t crc32_bitwise(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
	uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
	const uint8_t* current = (const uint8_t*)data;

	while (length-- != 0)
	{
		crc ^= *current++;

		for (int j = 0; j < 8; j++)
		{
			// branch-free
			crc = (crc >> 1) ^ (-int32_t(crc & 1) & Polynomial);

			// branching, much slower:
			//if (crc & 1)
			//  crc = (crc >> 1) ^ Polynomial;
			//else
			//  crc =  crc >> 1;
		}
	}

	return ~crc; // same as crc ^ 0xFFFFFFFF
}

uint32_t crc32_16bytes_prefetch(const void* data, size_t length, uint32_t previousCrc32 = 0, size_t prefetchAhead = 256)
{
	// CRC code is identical to crc32_16bytes (including unrolling), only added prefetching
	// 256 bytes look-ahead seems to be the sweet spot on Core i7 CPUs

	uint32_t crc = previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
	const uint32_t* current = (const uint32_t*)data;

	// enabling optimization (at least -O2) automatically unrolls the for-loop
	const size_t Unroll = 4;
	const size_t BytesAtOnce = 16 * Unroll;

	while (length >= BytesAtOnce + prefetchAhead)
	{
		PREFETCH(((const char*)current) + prefetchAhead);

		for (size_t unrolling = 0; unrolling < Unroll; unrolling++)
		{
#if __BYTE_ORDER == __BIG_ENDIAN
			uint32_t one = *current++ ^ swap(crc);
			uint32_t two = *current++;
			uint32_t three = *current++;
			uint32_t four = *current++;
			crc = Crc32Lookup[0][four & 0xFF] ^
				Crc32Lookup[1][(four >> 8) & 0xFF] ^
				Crc32Lookup[2][(four >> 16) & 0xFF] ^
				Crc32Lookup[3][(four >> 24) & 0xFF] ^
				Crc32Lookup[4][three & 0xFF] ^
				Crc32Lookup[5][(three >> 8) & 0xFF] ^
				Crc32Lookup[6][(three >> 16) & 0xFF] ^
				Crc32Lookup[7][(three >> 24) & 0xFF] ^
				Crc32Lookup[8][two & 0xFF] ^
				Crc32Lookup[9][(two >> 8) & 0xFF] ^
				Crc32Lookup[10][(two >> 16) & 0xFF] ^
				Crc32Lookup[11][(two >> 24) & 0xFF] ^
				Crc32Lookup[12][one & 0xFF] ^
				Crc32Lookup[13][(one >> 8) & 0xFF] ^
				Crc32Lookup[14][(one >> 16) & 0xFF] ^
				Crc32Lookup[15][(one >> 24) & 0xFF];
#else
			uint32_t one = *current++ ^ crc;
			uint32_t two = *current++;
			uint32_t three = *current++;
			uint32_t four = *current++;
			crc = Crc32Lookup[0][(four >> 24) & 0xFF] ^
				Crc32Lookup[1][(four >> 16) & 0xFF] ^
				Crc32Lookup[2][(four >> 8) & 0xFF] ^
				Crc32Lookup[3][four & 0xFF] ^
				Crc32Lookup[4][(three >> 24) & 0xFF] ^
				Crc32Lookup[5][(three >> 16) & 0xFF] ^
				Crc32Lookup[6][(three >> 8) & 0xFF] ^
				Crc32Lookup[7][three & 0xFF] ^
				Crc32Lookup[8][(two >> 24) & 0xFF] ^
				Crc32Lookup[9][(two >> 16) & 0xFF] ^
				Crc32Lookup[10][(two >> 8) & 0xFF] ^
				Crc32Lookup[11][two & 0xFF] ^
				Crc32Lookup[12][(one >> 24) & 0xFF] ^
				Crc32Lookup[13][(one >> 16) & 0xFF] ^
				Crc32Lookup[14][(one >> 8) & 0xFF] ^
				Crc32Lookup[15][one & 0xFF];
#endif
		}

		length -= BytesAtOnce;
	}

	const uint8_t* currentChar = (const uint8_t*)current;
	// remaining 1 to 63 bytes (standard algorithm)
	while (length-- != 0)
		crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

	return crc; // same as crc ^ 0xFFFFFFFF
}
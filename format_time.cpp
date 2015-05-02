#include <time.h>
#include <Windows.h>

#define HOUR (60 * MIN)
#define MIN (60 * SECOND)
#define SECOND (1 * CLOCKS_PER_SEC)

int format_time(char *output, clock_t time)
{
	int time_target = time;
	int hour = time_target / HOUR;
	int second = time_target % HOUR;
	int minute = second / MIN;
	second %= MIN;
	second /= SECOND;
	return wsprintfA(output, "%.2d:%.2d:%.2d", hour, minute, second);
}
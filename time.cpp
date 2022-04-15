#include <stdint.h>
#include <time.h>

uint64_t get_ms()
{
	struct timespec ts { 0 };

	clock_gettime(CLOCK_REALTIME, &ts);

	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

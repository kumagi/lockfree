#include <time.h>
#include <sys/time.h>

double gettime() __attribute__((always_inline));
double gettime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec*1e-6;
}

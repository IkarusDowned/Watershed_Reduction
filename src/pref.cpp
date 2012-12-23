#include "perf.hpp"

#ifdef _WIN32
#include <windows.h>

#endif


double getMillisecondsNow()
{

    double result = 0;
#ifdef _WIN32
    unsigned long long counter;
    static unsigned long long freq = 0;
    QueryPerformanceCounter((LARGE_INTEGER *)&counter);
    if(!freq)
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    result = (double)counter / (double)freq;
#endif
    return result;
}

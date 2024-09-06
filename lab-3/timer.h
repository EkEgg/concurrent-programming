#pragma once

#include <sys/time.h>

#define GetNow(destination) \
{ \
   struct timeval time; \
   gettimeofday(&time, NULL); \
   destination = time.tv_sec + 1e-6 * time.tv_usec; \
}

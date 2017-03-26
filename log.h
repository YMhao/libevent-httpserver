#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)

#define __DEBUG__
#ifdef __DEBUG__
#define LOG_DEBUG(format,...) fprintf(stdout, "<%s>|<%d>|<%s> " format "\n", __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_ERROR(format,...) fprintf(stderr, "[ERROR]<%s>|<%d>|<%s> " format "\n", __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define LOG_DEBUG(format,...)
#define LOG_ERROR(format,...)
#endif
#endif // LOG_H

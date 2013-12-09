#ifndef UTILS_H
#define UTILS_H
#include "config.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

/* macro tricks for logging functions to work */
#define STR(x) #x
#define TOSTR(x) STR(x)

#define LOGI(format, ...) do {\
                          time_t now = time(NULL);\
                          char timestr[20];\
                          strftime(timestr, 20, TIME_FORMAT, localtime(&now));\
                          fprintf(stderr, "\e[01;32m %s INFO: \e[0m" format "\n", timestr, ##__VA_ARGS__);}\
                          while(0)
#ifndef NDEBUG
#define LOGE(format, ...) do {\
                          time_t now = time(NULL);\
                          char timestr[20];\
                          strftime(timestr, 20, TIME_FORMAT, localtime(&now));\
                          fprintf(stderr, "\e[01;35m %s ERROR: \e[0m" format " on File: %s Line: %s\n", timestr, ##__VA_ARGS__, __FILE__, TOSTR(__LINE__));}\
                          while(0)
#else
#define LOGE(format, ...) do {\
                          time_t now = time(NULL);\
                          char timestr[20];\
                          strftime(timestr, 20, TIME_FORMAT, localtime(&now));\
                          fprintf(stderr, "\e[01;35m %s ERROR: \e[0m" format "\n", timestr, ##__VA_ARGS__);}\
                          while(0)
#endif
#ifndef NDEBUG
#define FATAL(format, ...) do {\
                          time_t now = time(NULL);\
                          char timestr[20];\
                          strftime(timestr, 20, TIME_FORMAT, localtime(&now));\
                          fprintf(stderr, "\e[01;31m %s FATAL: \e[0m" format " on File: %s Line: %s\n", timestr, ##__VA_ARGS__, __FILE__, TOSTR(__LINE__));exit(EXIT_FAILURE);}\
                          while(0)
#else
#define FATAL(format, ...) do {\
                          time_t now = time(NULL);\
                          char timestr[20];\
                          strftime(timestr, 20, TIME_FORMAT, localtime(&now));\
                          fprintf(stderr, "\e[01;31m %s FATAL: \e[0m" format "\n", timestr, ##__VA_ARGS__);exit(EXIT_FAILURE);}\
                          while(0)
#endif

/*
 * turn a string "x..y" to an integer pair
 */
std::pair<int, int> extract_pair(const char *s);

#endif /* !UTILS_H */


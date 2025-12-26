#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "mget.h"

void log_info(const char *format, ...) {
    printf("[ ");
    printf("\033[36m");
    printf("info");
    printf("\033[0m");
    printf(" ] ");
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    printf("\n");
}

void log_error(const char *format, ...) {
    fprintf(stderr, "[ ");
    fprintf(stderr, "\033[31m");
    fprintf(stderr, "error");
    fprintf(stderr, "\033[0m");
    fprintf(stderr, " ] ");
    
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
}

void log_success(const char *format, ...) {
    printf("[ ");
    printf("\033[32m");
    printf("success");
    printf("\033[0m");
    printf(" ] ");
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    printf("\n");
}

#pragma once

#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace seeds {

namespace log {

__attribute__ ((format(printf, 1, 2)))
static void d(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "[Debug  ] ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

__attribute__ ((format(printf, 1, 2)))
static void v(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "[Verbose] ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

}  // log

}  // seeds

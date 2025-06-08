#include<stdarg.h>
#include<stdio.h>
#include<assert.h>

void testing_assert(int expr, char *failMsgFormat, ...){
    if (!expr){
        va_list args;

        va_start(args, NULL);

        // For some reason stdout wasn't being flushed before
        fflush(stdout);
        fprintf(stderr, "\n\n");
        fprintf(stderr, "\033[1;31m=-=-=-=-=-=-=-=-=-=-=\n");
        fprintf(stderr, "    TESTS FAILED\n");
        fprintf(stderr, "=-=-=-=-=-=-=-=-=-=-=\n\n");
        fprintf(stderr, "\033[0;31m<Assert Error>: Assertion failed\n\n");
        fprintf(stderr, "\033[0;37m");
        vfprintf(stderr, failMsgFormat, args);
        fprintf(stderr, "\n\n");

        va_end(args);
    } 

    assert(expr);
}




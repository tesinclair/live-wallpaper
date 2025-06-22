/*

    Defines macros for debug logging

    All macros are defined by default in DEBUG mode unless the SILENT flag has been triggered (see ./autogen --help)
    All macros are undefined by default in Release mode unless the VERBOSE flag has been triggered (see above)
    
    If you would like to turn on logging perminantly no matter what, change the 0 to a 1 in the first following #if directive

*/

#pragma once

#include <stdio.h>

// If debug or verbose are defined, but silent is not defined.
// Change the 0 to a 1 to perminanly turn on logging in spite of flags.
#if ((defined(DEBUG) || defined(VERBOSE)) && !defined(SILENT)) || 0 

/**
   @param namespace: the namespace or class the code errs in
   @param errno: the error number (defined in bg_types.h)
   @param errmsg: the error message to print
 **/
#define LOG_ERROR(namespace, errno, errmsg) \
    fprintf(stderr, "[ERROR: <%s<%ld>>]: %s\n", namespace, errno, errmsg); \
    fflush(stderr);

/**
   @param namespace: the namespace or class the code logs in
   @param format: the formatted string to print as info
   @param __VA_ARGS__: The variadic arguments for the format string
 **/
#define LOG_INFO(namespace, format, ...) \
    printf("[INFO: <%s>]: ", namespace); \
    printf(format, __VA_ARGS__); \
    printf("\n");

/**
   @param namespace: the namespace or class the code logs in
   @param format: the formatted string to print as warn 
   @param __VA_ARGS__: The variadic arguments for the format string
 **/
#define LOG_WARN(namespace, format, ...) \
    printf("[WARN: <%s>]: ", namespace); \
    printf(format, __VA_ARGS__); \
    printf("\n");

#else

#define LOG_ERROR(namespace, errno, errmsg)
#define LOG_INFO(namespace, format, ...)
#define LOG_WARN(namespace, format, ...)

#endif



#pragma once

#include <string>

enum ErrNo{
    NOERR = 0,
    ALLOCATION_FAILED = 100,
    BAD_POINTER,
    BAD_ARGS,
    FAILED_SAFE_SHUTDOWN,
    FAILED_INSTANCE_LOCK,
    FAILED_AQUIRE_SEMAPHORE,
};

struct err{
    const char* errContext;
    size_t errno;
    bool err;
    const char* errMsg;
};

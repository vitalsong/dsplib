#pragma once

#include <dsplib/defs.h>

#ifdef DSPLIB_NO_EXCEPTIONS

#include <iostream>

//TODO: add filename to message
#define DSPLIB_THROW(MSG)                                                                                              \
    std::cerr << (std::string("dsplib: ") + MSG) << std::endl;                                                         \
    std::abort();

#else

#include <stdexcept>

#define DSPLIB_THROW(MSG) throw std::runtime_error(std::string("dsplib: ") + std::string(MSG));

#endif

#define DSPLIB_ASSERT(condition, message)                                                                              \
    if (!(condition)) {                                                                                                \
        DSPLIB_THROW(message);                                                                                         \
    }

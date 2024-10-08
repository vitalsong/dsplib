#pragma once

#include <dsplib/defs.h>
#include <cassert>

#ifdef DSPLIB_NO_EXCEPTIONS

#include <iostream>

//TODO: add filename to message
#define DSPLIB_THROW(MSG)                                                                                              \
    std::cerr << (std::string("dsplib: ") + MSG) << std::endl;                                                         \
    std::abort();

#else

#include <stdexcept>

#define DSPLIB_THROW(MSG) throw std::runtime_error(std::string("dsplib: ") + MSG);

#endif

#define DSPLIB_ASSERT(condition, message)                                                                              \
    if (!(condition)) {                                                                                                \
        DSPLIB_THROW(message);                                                                                         \
    }

#ifdef _MSC_VER
#define DSPLIB_ASSUME(cond)                                                                                            \
    assert(cond);                                                                                                      \
    __assume(cond);
#elif defined(__clang__)
#define DSPLIB_ASSUME(cond)                                                                                            \
    assert(cond);                                                                                                      \
    __builtin_assume(cond)
#elif defined(__GNUC__)
#define DSPLIB_ASSUME(cond)                                                                                            \
    assert(cond);                                                                                                      \
    ((cond) ? static_cast<void>(0) : __builtin_unreachable());
#else
#define DSPLIB_ASSUME(cond)                                                                                            \
    assert(cond);                                                                                                      \
    static_cast<void>((cond) ? 0 : 0);
#endif
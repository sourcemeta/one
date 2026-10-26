#ifndef SOURCEMETA_ONE_HTTP_UWEBSOCKETS_H
#define SOURCEMETA_ONE_HTTP_UWEBSOCKETS_H

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wnewline-eof"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
// Give the time helper this library declares at namespace scope inline
// linkage, so that every translation unit including it does not emit a copy
// NOLINTNEXTLINE(readability-identifier-naming)
#define time_ms inline time_ms
#include <src/App.h>
#undef time_ms
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif

#pragma once

#include <assert.h>
#include <stdint.h>

#define DISABLE_COPY(Class)        \
    Class(const Class &) = delete; \
    Class &operator=(const Class &) = delete;

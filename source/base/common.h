#pragma once

#include <assert.h>
#include <stdint.h>

#include <string>

#define DISABLE_COPY(Class)        \
    Class(const Class &) = delete; \
    Class &operator=(const Class &) = delete;

std::wstring to_wstring(const std::string &s);
std::string to_string(const std::wstring &s);

namespace aspia
{

extern const int kDefaultHostTcpPort;

} // namespace

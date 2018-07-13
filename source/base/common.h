#pragma once

#include <assert.h>
#include <stdint.h>

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

#define DISABLE_COPY(Class)        \
    Class(const Class &) = delete; \
    Class &operator=(const Class &) = delete;

ASPIA_BASE_API
std::wstring to_wstring(const std::string &s);

ASPIA_BASE_API
std::string to_string(const std::wstring &s);

namespace aspia
{

inline const int kDefaultHostTcpPort = 8050;

} // namespace

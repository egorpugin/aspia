#include "../common.h"

#include <codecvt>

static auto& get_string_converter()
{
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter;
}

std::wstring to_wstring(const std::string &s)
{
    auto &converter = get_string_converter();
    return converter.from_bytes(s.c_str());
}

std::string to_string(const std::wstring &s)
{
    auto &converter = get_string_converter();
    return converter.to_bytes(s.c_str());
}

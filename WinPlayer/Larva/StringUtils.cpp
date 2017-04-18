#include "StringUtils.h"
#include <codecvt>

static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

std::wstring StringUtils::string2wstring(const std::string s)
{
    return converter.from_bytes(s);
}

LPCWSTR StringUtils::string2LPCWSTR(const std::string s)
{
    return StringUtils::string2wstring(s).c_str();
}

std::string StringUtils::wstring2string(const std::wstring s)
{
    return converter.to_bytes(s);
}

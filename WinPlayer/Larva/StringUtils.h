#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <sstream>
#include <windows.h>
using namespace std;

class StringUtils
{
public:
    template <typename T>
    static string convert(T&& value){
        stringstream stream;
        stream << value;
        return stream.str();
    }

    static std::wstring string2wstring(const std::string s);

    static LPCWSTR string2LPCWSTR(const std::string s);

    static std::string wstring2string(const std::wstring s);
};


#endif
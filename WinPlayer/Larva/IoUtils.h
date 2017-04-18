#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

class IoUtils
{
public:
    // write to file
    template <typename T>
    static void toFile(std::string&& fileName, const std::vector<T>& v) {
        std::ofstream out(fileName);
        if (out.is_open())
        {
            for (const auto& e : v)
                out << e << endl;
            out.close();
        }
    }

    // read from file
    static std::vector<std::string> fromFile(std::string& fileName);
};





#endif
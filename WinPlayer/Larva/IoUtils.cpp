#include "IoUtils.h"

std::vector<std::string> IoUtils::fromFile(std::string& fileName) {
    std::ifstream in(fileName);
    std::string data;
    std::vector<std::string> results;
    if (!in.is_open())
    {
        std::cout << "Error opening file" << std::endl;
    }
    while (getline(in, data))
    {
        if (data.size() > 0)
            results.push_back(data);
    }
    in.close();
    return results;
}
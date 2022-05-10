#include "test.h"

#include "ulib.h"
#include <fstream>
#include <sstream>

namespace ul::test
{
    auto get_lines(std::string&&path) -> std::vector<std::string>
    {
        auto ifs = std::ifstream(path);
        if (!ifs)
        {
            ::ul::error("Cannot find " + path);
            return {};
        }

        auto lines = std::vector<std::string>{};
        std::string line;
        while(std::getline(ifs, line))
        {
            lines.emplace_back(line);
        }

        return lines;
    }
}
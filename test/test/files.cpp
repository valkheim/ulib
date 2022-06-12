#include <fstream>
#include <sstream>

#include "test.h"
#include "ul.h"


namespace ul::test
{
  auto get_lines(std::string&& path) -> std::vector<std::string>
  {
    auto ifs = std::ifstream(path);
    if (!ifs) {
      ::ul::error("Cannot find " + path);
      return {};
    }

    auto lines = std::vector<std::string>{};
    std::string line;
    while (std::getline(ifs, line)) {
      lines.emplace_back(line);
    }

    return lines;
  }
}  // namespace ul::test
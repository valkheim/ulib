#include "utils.h"

namespace ul
{
  auto base_name(std::string const& path) -> std::string { return path.substr(path.find_last_of("/\\") + 1); }
}  // namespace ul
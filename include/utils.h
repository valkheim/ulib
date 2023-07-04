#pragma once

#include <string>

// no other private include here

namespace ul
{
  enum class walk_t : bool { WALK_CONTINUE, WALK_STOP };

  auto base_name(std::string const &path) -> std::string;
  auto from_ansi_wstring_to_string(std::wstring in) -> std::string;
  auto from_string_to_wstring(std::string in) -> std::wstring;
}  // namespace ul
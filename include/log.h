#pragma once

#include <Windows.h>

#include <string_view>

namespace ul
{
  auto error(const std::string_view& message) -> DWORD;
  void warning(const std::string_view& message);
  void info(const std::string_view& message);
  VOID hexdump(PVOID const buffer, SIZE_T const buffer_size);
  VOID hexdump(std::string_view&& caption, PVOID const buffer, SIZE_T const buffer_size);
}  // namespace ul
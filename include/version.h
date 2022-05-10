#pragma once

#include <optional>
#include <string>

namespace ul
{
  struct WindowsBuildVersion {
    std::uint32_t a;
    std::uint32_t b;
    std::uint32_t c;
    std::uint32_t d;
    auto to_string() const -> std::string;
  };

  auto get_ulib_version() -> std::string;
  auto get_windows_build_version() -> std::optional<WindowsBuildVersion>;
};  // namespace ul
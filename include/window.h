#pragma once

#include <Windows.h>

#include <functional>
#include <optional>
#include <vector>

#include "utils.h"

namespace ul
{
  struct Window {
    HWND handle;
    HWND parent;  // may be NULL
    std::optional<std::string> title;
  };

  using Windows = std::vector<::ul::Window>;
  using on_window = std::function<::ul::walk_t(::ul::Window)>;

  [[nodiscard]] auto walk_top_windows(on_window callback) -> bool;
  [[nodiscard]] auto walk_child_windows(HWND hWnd, on_window callback) -> bool;
  [[nodiscard]] auto walk_windows(on_window callback) -> bool;
  auto with_window(std::string_view&& requested_title, on_window callback) -> bool;
  auto count_windows() -> size_t;

}  // namespace ul
#include "window.h"

#include "utils.h"

namespace ul
{
  namespace
  {
    auto get_window(HWND hWnd) -> ::ul::Window
    {
      auto length = GetWindowTextLengthA(hWnd);
      auto title = std::string{};
      title.resize(length + 1);
      GetWindowTextA(hWnd, title.data(), length + 1);
      return ::ul::Window{hWnd, GetParent(hWnd), std::move(title)};
    }
  }  // namespace

  auto walk_top_windows(on_window callback) -> bool
  {
    return EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
          auto callback = (::ul::on_window*)lParam;
          return (*callback)(get_window(hWnd)) == ::ul::walk_t::WALK_CONTINUE;
        },
        (LPARAM)&callback);
  }

  auto walk_child_windows(HWND hWnd, on_window callback) -> bool
  {
    return EnumChildWindows(
        hWnd,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
          auto callback = (::ul::on_window*)lParam;
          return (*callback)(get_window(hWnd)) == ::ul::walk_t::WALK_CONTINUE;
        },
        (LPARAM)&callback);
  }

  auto walk_windows(on_window callback) -> bool
  {
    std::function<::ul::walk_t(::ul::Window const&, on_window)> wc;
    wc = [&](::ul::Window const& window, on_window callback) -> ::ul::walk_t {
      return (callback(window) == ::ul::walk_t::WALK_CONTINUE &&
              ::ul::walk_child_windows(window.handle, [&](::ul::Window const& window) -> ::ul::walk_t { return wc(window, callback); }) == true)
                 ? ::ul::walk_t::WALK_CONTINUE
                 : ::ul::walk_t::WALK_STOP;
    };

    return ::ul::walk_top_windows([&](::ul::Window window) -> ::ul::walk_t {
      wc(window, callback);
      return ::ul::walk_t::WALK_CONTINUE;
    });
  }

  auto with_window(std::string_view&& requested_title, on_window callback) -> bool
  {
    auto found = false;
    (void)::ul::walk_windows([&](::ul::Window window) -> ::ul::walk_t {
      if (!window.title) {
        return ::ul::walk_t::WALK_CONTINUE;
      }

      if (*window.title != requested_title) {
        return ::ul::walk_t::WALK_CONTINUE;
      }

      found = true;
      return callback(window);
    });

    return found;
  }

  auto count_windows() -> size_t
  {
    size_t count = 0;
    (void)::ul::walk_windows([&](::ul::Window const& window) -> ::ul::walk_t {
      count++;
      return ::ul::walk_t::WALK_CONTINUE;
    });

    return count;
  }
}  // namespace ul
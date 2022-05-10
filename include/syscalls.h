#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "utils.h"

namespace ul
{
  constexpr std::size_t g_regular_syscall_function_size_x64 = 0x20;
  constexpr std::size_t g_regular_syscall_function_size_x86 = 0x10;
#if _WIN64
  constexpr auto g_regular_syscall_function_size = g_regular_syscall_function_size_x64;
#else
  constexpr auto g_regular_syscall_function_size = g_regular_syscall_function_size_x86;
#endif

  struct Syscall {
    bool hooked;
    std::uint32_t number;
    void* address;
    std::string name;
    auto to_string() const -> std::string;
  };

  using Syscalls = std::vector<::ul::Syscall>;
  using on_syscall = std::function<::ul::walk_t(::ul::Syscall const& syscall)>;

  auto walk_syscalls(on_syscall callback) -> bool;
  auto walk_syscalls_x86(on_syscall&& callback) -> bool;
  auto walk_syscalls_x64(on_syscall&& callback) -> bool;
  void show_syscalls();
  [[nodiscard]] auto with_syscall(std::string_view&& requested_name, std::function<void(::ul::Syscall const& syscall)> callback) -> bool;
  auto get_syscalls() -> Syscalls;
}  // namespace ul
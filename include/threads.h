#pragma once

#include <Windows.h>

#include <functional>
#include <vector>

#include "processes.h"
#include "utils.h"

namespace ul
{
  using Tid = DWORD;

  struct Thread {
    ::ul::Process process;
    Tid tid;
  };

  using Threads = std::vector<::ul::Thread>;
  using on_thread = std::function<::ul::walk_t(::ul::Thread const& thread)>;

  auto walk_threads_using_toolhelp(std::string_view&& requested_process_name, on_thread callback) -> bool;
  auto walk_threads_using_toolhelp(::ul::Pid const requested_pid, on_thread callback) -> bool;
}  // namespace ul
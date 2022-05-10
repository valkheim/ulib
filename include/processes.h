#pragma once

#include <Windows.h>

#include <functional>
#include <optional>
#include <vector>

#include "utils.h"

namespace ul
{
  using Pid = DWORD;

  struct Process {
    Pid pid;
    std::optional<std::string> path;
    std::optional<std::string> name;
    void* custom;
  };

  using Processes = std::vector<Process>;
  using on_process = std::function<::ul::walk_t(::ul::Process)>;

  void walk_processes_ids_using_enumprocess(on_process callback);
  void walk_processes_ids_using_toolhelp(on_process callback);
  void walk_processes_ids_using_wts(on_process callback);
  auto get_processes_ids_using_enumprocess() -> ::ul::Processes;
  auto get_processes_ids_using_toolhelp() -> ::ul::Processes;
  auto get_processes_ids_using_wts() -> ::ul::Processes;
  [[nodiscard]] auto with_process_using_enumprocess(std::string_view&& requested_name, on_process callback) -> bool;
  [[nodiscard]] auto with_process_using_toolhelp(::ul::Pid const requested_pid, on_process callback) -> bool;
  [[nodiscard]] auto with_process_using_toolhelp(std::string_view&& requested_name, on_process callback) -> bool;
  [[nodiscard]] auto with_process_using_wts(std::string_view&& requested_name, on_process callback) -> bool;
}  // namespace ul
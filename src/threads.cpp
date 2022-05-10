#include "threads.h"

#include <tlhelp32.h>  // CreateToolhelp32Snapshot, …
#pragma comment(lib, "wtsapi32.lib")

#include "log.h"

namespace ul
{
  namespace
  {
    auto search_threads(::ul::Process const &process, on_thread callback) -> ::ul::walk_t
    {
      THREADENTRY32 te{sizeof(THREADENTRY32)};

      auto snapshot = static_cast<HANDLE>(process.custom);
      if (::Thread32First(snapshot, &te) == FALSE) {
        ::ul::error("Cannot Thread32First");
        return ::ul::walk_t::WALK_STOP;
      }

      do {
        if (te.th32OwnerProcessID != process.pid) continue;

        auto thread = ::ul::Thread{process, te.th32ThreadID};
        if (callback(thread) == ::ul::walk_t::WALK_STOP) break;
      } while (::Thread32Next(snapshot, &te));
      return ::ul::walk_t::WALK_STOP;
    }

  }  // namespace

  auto walk_threads_using_toolhelp(::ul::Pid const requested_pid, on_thread callback) -> bool
  {
    return ::ul::with_process_using_toolhelp(requested_pid,
                                             [&](::ul::Process const &process) -> ::ul::walk_t { return ::ul::search_threads(process, callback); });
  }

  auto walk_threads_using_toolhelp(std::string_view &&requested_process_name, on_thread callback) -> bool
  {
    THREADENTRY32 te{sizeof(THREADENTRY32)};
    return ::ul::with_process_using_toolhelp(std::move(requested_process_name), [&](::ul::Process const &process) -> ::ul::walk_t {
      auto snapshot = static_cast<HANDLE>(process.custom);
      if (::Thread32First(snapshot, &te) == FALSE) {
        ::ul::error("Cannot Thread32First");
        return ::ul::walk_t::WALK_STOP;
      }

      do {
        if (te.th32OwnerProcessID != process.pid) continue;

        auto thread = ::ul::Thread{process, te.th32ThreadID};
        if (callback(thread) == ::ul::walk_t::WALK_STOP) break;
      } while (::Thread32Next(snapshot, &te));
      return ::ul::walk_t::WALK_STOP;
    });
  }

}  // namespace ul
#include "threads.h"

#include <tlhelp32.h>  // CreateToolhelp32Snapshot, …
#pragma comment(lib, "wtsapi32.lib")

#include "log.h"
#include "nt.h"
#include "processes.h"

namespace ul
{
  namespace
  {
    auto search_threads_using_tlhelp32(::ul::Process const &process, on_thread callback) -> ::ul::walk_t
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

  void show_thread(::ul::Thread const *thread)
  {
    ::ul::show_process(&(thread->process));
    printf("Thread.tid: %d\n", thread->tid);
  }

  auto walk_threads_using_toolhelp(::ul::Pid const requested_pid, on_thread callback) -> bool
  {
    return ::ul::with_process_using_toolhelp(requested_pid,
                                             [&](::ul::Process const &process) -> ::ul::walk_t { return ::ul::search_threads_using_tlhelp32(process, callback); });
  }

  auto walk_threads_using_toolhelp(std::string_view &&requested_process_name, on_thread callback) -> bool
  {
    return ::ul::with_process_using_toolhelp(std::move(requested_process_name), [&](::ul::Process const &process) -> ::ul::walk_t {
       return ::ul::search_threads_using_tlhelp32(process, callback);
    });
  }

  auto walk_threads_using_ntgetnextthread(::ul::Pid const requested_pid, on_thread callback) -> bool
  {
    typedef NTSTATUS (NTAPI * _NtGetNextThread)(
			_In_ HANDLE ProcessHandle,
			_In_ HANDLE ThreadHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ ULONG HandleAttributes,
			_In_ ULONG Flags,
			_Out_ PHANDLE NewThreadHandle
    );
    auto NtGetNextThread = reinterpret_cast<_NtGetNextThread>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtGetNextThread"));
    if (!NtGetNextThread)
      return false;

    auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, requested_pid);
    if (hProcess == INVALID_HANDLE_VALUE)
      return false;

    auto hThread = INVALID_HANDLE_VALUE;
    auto hThreadNext = hThread;
    for (;;) {
      auto status = NtGetNextThread(hProcess, hThread, THREAD_QUERY_INFORMATION, 0, 0, &hThreadNext);
      CloseHandle(hThread);
      if (!NT_SUCCESS(status)) { // STATUS_NO_MORE_ENTRIES
        break;
      }

      puts("got thread");
      ::ul::Thread thread;
      thread.process = ::ul::get_process_from_handle(hProcess, nullptr);
      thread.tid = GetThreadId(hThread);
      // GetThreadPriority(hThread)
      if (callback(thread) == ::ul::walk_t::WALK_STOP)
        break;

      hThread = hThreadNext;
    }

    CloseHandle(hProcess);
    return true;
  }
}  // namespace ul
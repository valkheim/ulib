#include "handles.h"

#include "ntquerysysteminformation.hpp"
#include "utils.h"

namespace ul
{
  namespace
  {
    auto get_handles_infos() -> PSYSTEM_HANDLE_INFORMATION { return ::ul::get_system_informations<PSYSTEM_HANDLE_INFORMATION>(); }
  }  // namespace

  VOID walk_handles(std::function<::ul::walk_t(SYSTEM_HANDLE_TABLE_ENTRY_INFO *)> callback)
  {
    auto handles = ::ul::get_handles_infos();
    for (unsigned i = 0; i < handles->NumberOfHandles; ++i) {
      if (callback(&handles->Handles[i]) == ::ul::walk_t::WALK_STOP) break;
    }

    VirtualFree(handles, 0, MEM_RELEASE);
  }

  auto with_handle(USHORT requested_pid, std::function<VOID(PSYSTEM_HANDLE_TABLE_ENTRY_INFO)> callback) -> BOOL
  {
    auto found = false;
    ::ul::walk_handles([&](PSYSTEM_HANDLE_TABLE_ENTRY_INFO handle) -> ::ul::walk_t {
      if (handle->UniqueProcessId == requested_pid) {
        callback(handle);
        found = true;
        return ::ul::walk_t::WALK_STOP;
      }

      return ::ul::walk_t::WALK_CONTINUE;
    });

    return found;
  }

  VOID show_handle(PSYSTEM_HANDLE_TABLE_ENTRY_INFO const handle)
  {
    printf("Handle UniqueProcessId       %d\n", handle->UniqueProcessId);
    printf("Handle CreatorBackTraceIndex 0x%04x\n", handle->CreatorBackTraceIndex);
    printf("Handle ObjectTypeIndex       0x%02x\n", handle->ObjectTypeIndex);
    printf("Handle HandleAttributes      0x%02x\n", handle->HandleAttributes);
    printf("Handle HandleValue           0x%04x\n", handle->HandleValue);
    printf("Handle Object                0x%p\n", handle->Object);
    printf("Handle GrantedAccess         0x%08lx\n", handle->GrantedAccess);
  }

  VOID show_handles()
  {
    unsigned i = 0;
    ::ul::walk_handles([&i](PSYSTEM_HANDLE_TABLE_ENTRY_INFO handle) -> ::ul::walk_t {
      printf("[%d]\n", i++);
      show_handle(handle);
      return ::ul::walk_t::WALK_CONTINUE;
    });
  }
};  // namespace ul
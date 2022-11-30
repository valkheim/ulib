#include <windows.h>

#include <functional>
#include <string_view>

#include "nt.h"
#include "utils.h"

namespace ul
{
  VOID walk_handles(std::function<::ul::walk_t(SYSTEM_HANDLE_TABLE_ENTRY_INFO *)> callback);
  [[nodiscard]] auto with_handle(USHORT requested_pid, std::function<VOID(PSYSTEM_HANDLE_TABLE_ENTRY_INFO)> callback) -> BOOL;
  VOID show_handle(PSYSTEM_HANDLE_TABLE_ENTRY_INFO const handle);
  VOID show_handles();
}  // namespace ul
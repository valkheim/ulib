#include <windows.h>

#include <functional>
#include <string_view>

#include "nt.h"
#include "utils.h"

namespace ul
{
  VOID walk_modules(std::function<::ul::walk_t(SYSTEM_MODULE *)> callback);
  [[nodiscard]] auto with_module(std::string_view &&requested_name, std::function<VOID(PSYSTEM_MODULE)> callback) -> BOOL;
  VOID show_module(PSYSTEM_MODULE const module);
  VOID show_modules();
}  // namespace ul
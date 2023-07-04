#include <windows.h>

#include <functional>
#include <optional>
#include <string_view>

#include "nt.h"
#include "utils.h"

namespace ul
{
  struct Module {
    std::optional<std::string> name;
    std::optional<std::string> path;
    void *base;
    std::size_t size;
  };

  using on_module = std::function<::ul::walk_t(::ul::Module *)>;

  void walk_modules_using_ntquerysysteminformation(on_module callback); // NtQuerySystemInformation
  void walk_modules_using_enumerateloadedmodules(on_module callback); // EnumerateLoadedModules
  void walk_modules_using_ldrenumerateloadedmodules(on_module callback); // LdrEnumerateLoadedModules
  [[nodiscard]] auto with_module_using_ntquerysysteminformation(std::string_view &&requested_name, on_module callback) -> bool;
  void show_module(::ul::Module const *module);
  void show_modules();
}  // namespace ul
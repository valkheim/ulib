#include "modules.h"

#include "ntquerysysteminformation.hpp"
#include "utils.h"

namespace ul
{
  namespace
  {
    auto get_modules_infos() -> PSYSTEM_MODULE_INFORMATION { return ::ul::get_system_informations<PSYSTEM_MODULE_INFORMATION>(); }
  }  // namespace

  VOID walk_modules(std::function<::ul::walk_t(SYSTEM_MODULE *)> callback)
  {
    auto modules = ::ul::get_modules_infos();
    for (unsigned i = 0; i < modules->ModulesCount; ++i) {
      if (callback(&modules->Modules[i]) == ::ul::walk_t::WALK_STOP) break;
    }

    VirtualFree(modules, 0, MEM_RELEASE);
  }

  auto with_module(std::string_view &&requested_name, std::function<VOID(PSYSTEM_MODULE)> callback) -> BOOL
  {
    auto found = false;
    ::ul::walk_modules([&](PSYSTEM_MODULE module) -> ::ul::walk_t {
      if (_stricmp(module->Name + module->NameOffset, requested_name.data()) == 0) {
        callback(module);
        found = true;
        return ::ul::walk_t::WALK_STOP;
      }

      return ::ul::walk_t::WALK_CONTINUE;
    });

    return found;
  }

  VOID show_module(PSYSTEM_MODULE const module)
  {
    printf("Image name %s\n", module->Name + module->NameOffset);
    printf("Image path %s\n", module->Name);
    printf("Image base 0x%p\n", module->ImageBaseAddress);
    printf("Image size 0x%x\n", module->ImageSize);
  }

  VOID show_modules()
  {
    unsigned i = 0;
    ::ul::walk_modules([&i](PSYSTEM_MODULE module) -> ::ul::walk_t {
      printf("[%d]\n", i++);
      show_module(module);
      return ::ul::walk_t::WALK_CONTINUE;
    });
  }
}  // namespace ul
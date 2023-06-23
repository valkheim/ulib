#include "modules.h"

#include "ntquerysysteminformation.hpp"
#include "utils.h"

namespace ul
{
  namespace
  {
    // EnumerateLoadedModules
    auto get_modules_infos_using_ntquerysysteminformation() -> PSYSTEM_MODULE_INFORMATION { return ::ul::get_system_informations<PSYSTEM_MODULE_INFORMATION>(); }

    auto get_module_from_ntquerysysteminformation_data(PSYSTEM_MODULE ntqsi_module) -> ::ul::Module {
      auto mod = ::ul::Module{};
      mod.name = std::nullopt;
      if (ntqsi_module->Name + ntqsi_module->NameOffset)
        mod.name = std::string(ntqsi_module->Name + ntqsi_module->NameOffset);

      mod.path = std::nullopt;
      if (ntqsi_module->Name)
        mod.path = std::string(ntqsi_module->Name);

      mod.base = ntqsi_module->ImageBaseAddress;
      mod.size = ntqsi_module->ImageSize;
      return mod;
    }
  }  // namespace

  void walk_modules_using_ntquerysysteminformation(on_module callback)
  {
    auto modules = ::ul::get_modules_infos_using_ntquerysysteminformation();
    for (unsigned i = 0; i < modules->ModulesCount; ++i) {
      auto module = ::ul::get_module_from_ntquerysysteminformation_data(&modules->Modules[i]);
      if (callback(&module) == ::ul::walk_t::WALK_STOP) break;
    }

    VirtualFree(modules, 0, MEM_RELEASE);
  }

  auto with_module(std::string_view &&requested_name, on_module callback) -> bool
  {
    auto found = false;
    ::ul::walk_modules_using_ntquerysysteminformation([&](::ul::Module *module) -> ::ul::walk_t {
      if (!module->name) {
        return ::ul::walk_t::WALK_CONTINUE;
      }

      if (_stricmp(module->name->data(), requested_name.data()) != 0) {
        return ::ul::walk_t::WALK_CONTINUE;
      }

      found = true;
      return callback(module);
    });

    return found;
  }

  void show_module(::ul::Module const *module)
  {
    if (module->name)
      printf("Image name: %s\n", module->name->data());
    else
      puts("Image name: (none)");

    if (module->path)
      printf("Image path: %s\n", module->path->data());
    else
      puts("Image path: (none)");

    printf("Image base: 0x%p\n", module->base);
    printf("Image size: 0x%x\n", module->size);
  }

  void show_modules()
  {
    unsigned i = 0;
    ::ul::walk_modules_using_ntquerysysteminformation([&i](::ul::Module const *module) -> ::ul::walk_t {
      printf("[%d]\n", i++);
      show_module(module);
      return ::ul::walk_t::WALK_CONTINUE;
    });
  }
}  // namespace ul
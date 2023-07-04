#include "modules.h"

#pragma comment (lib, "dbghelp.lib") // EnumerateLoadedModules
#include <dbghelp.h>

#include "ntquerysysteminformation.hpp"
#include "utils.h"

namespace ul
{
  namespace
  {
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

    auto get_module_from_enumerateloadedmodules_data(PSTR ModuleName, ULONG ModuleBase, ULONG ModuleSize) -> ::ul::Module
    {
      auto mod = ::ul::Module{};
      mod.name = std::nullopt;
      mod.path = std::nullopt;
      if (ModuleName)
        mod.path = std::string(ModuleName); // todo: split to get name

      mod.base = reinterpret_cast<void*>(ModuleBase);
      mod.size = ModuleSize;
      return mod;
    }

    BOOL CALLBACK enumerate_modules_callback(PSTR ModuleName, ULONG ModuleBase, ULONG ModuleSize, PVOID UserContext)
    {
        auto *callback = (::ul::on_module *)(UserContext);
        auto module = get_module_from_enumerateloadedmodules_data(ModuleName, ModuleBase, ModuleSize);
        return ((*callback)(&module) != ::ul::walk_t::WALK_STOP);
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

  void walk_modules_using_enumerateloadedmodules(on_module callback)
  {
    // EnumerateLoadedModules
    // EnumerateLoadedModules64
    // EnumerateLoadedModulesEx
    // EnumerateLoadedModulesExW
    // EnumerateLoadedModulesW64
    EnumerateLoadedModules(GetCurrentProcess(), reinterpret_cast<PENUMLOADED_MODULES_CALLBACK>(enumerate_modules_callback), &callback);
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
      printf("Module.name: %s\n", module->name->data());
    else
      puts("Module.name: (none)");

    if (module->path)
      printf("Module.path: %s\n", module->path->data());
    else
      puts("Module.path: (none)");

    printf("Module.base: 0x%p\n", module->base);
    printf("Module.size: 0x%x\n", module->size);
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
#include "modules.h"

#pragma comment (lib, "dbghelp.lib") // EnumerateLoadedModules
#include <dbghelp.h>

#include <locale>
#include <codecvt>

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
      {
        mod.path = std::string(ModuleName);
        mod.name = ::ul::base_name(*(mod.path));
      }

      mod.base = reinterpret_cast<void*>(ModuleBase);
      mod.size = ModuleSize;
      return mod;
    }

    BOOL CALLBACK enumerate_modules_callback(_In_ PSTR ModuleName, _In_ ULONG ModuleBase, _In_ ULONG ModuleSize,  _In_opt_ PVOID UserContext)
    {
        auto *callback = (::ul::on_module *)(UserContext);
        auto module = get_module_from_enumerateloadedmodules_data(ModuleName, ModuleBase, ModuleSize);
        return ((*callback)(&module) != ::ul::walk_t::WALK_STOP);
    }

    auto get_module_from_ldr_data_table_entry(LDR_DATA_TABLE_ENTRY *data_table_entry) -> ::ul::Module
    {
      auto mod = ::ul::Module{};
      mod.name = std::nullopt;
      mod.path = std::nullopt;
      if (!data_table_entry)
        return mod;

      if (data_table_entry->FullDllName.Buffer)
      {
        auto wstring = std::wstring(data_table_entry->FullDllName.Buffer, data_table_entry->FullDllName.Length / 2);
        mod.path = ::ul::from_ansi_wstring_to_string(wstring);
        mod.name = ::ul::base_name(*(mod.path));
      }

      mod.base = data_table_entry->DllBase;
      mod.size = data_table_entry->SizeOfImage;
      return mod;
    }

    VOID NTAPI ldr_enumerate_modules_callback(_In_ LDR_DATA_TABLE_ENTRY *DataTableEntry, _In_ PVOID Context, _Inout_ BOOLEAN *StopEnumeration)
    {
        auto *callback = (::ul::on_module *)(Context);
        auto module = get_module_from_ldr_data_table_entry(DataTableEntry);
        *StopEnumeration = ((*callback)(&module) == ::ul::walk_t::WALK_STOP);

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

  void walk_modules_using_ldrenumerateloadedmodules(on_module callback)
  {
    typedef VOID(NTAPI *PLDR_LOADED_MODULE_ENUMERATION_CALLBACK_FUNCTION)(
      _In_    LDR_DATA_TABLE_ENTRY *DataTableEntry,
      _In_    PVOID Context,
      _Inout_ BOOLEAN *StopEnumeration
    );
    typedef NTSTATUS (NTAPI * _LdrEnumerateLoadedModules)(
      _In_opt_ ULONG Flags,
      _In_ PLDR_LOADED_MODULE_ENUMERATION_CALLBACK_FUNCTION CallbackFunction,
      _In_opt_ PVOID Context
    );
    auto LdrEnumerateLoadedModules = reinterpret_cast<_LdrEnumerateLoadedModules>(GetProcAddress(GetModuleHandle("ntdll.dll"), "LdrEnumerateLoadedModules"));
    LdrEnumerateLoadedModules(false, reinterpret_cast<PLDR_LOADED_MODULE_ENUMERATION_CALLBACK_FUNCTION>(ldr_enumerate_modules_callback), &callback);
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
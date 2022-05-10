#pragma once

#include "nt.h"
#include "addresses.h"

namespace ul
{

  template <typename T>
  struct SystemInformationClass : std::false_type {
  };
  template <>
  struct SystemInformationClass<SYSTEM_PROCESS_INFORMATION> {
    static constexpr auto value = SystemModuleInformation;
  };
  template <>
  struct SystemInformationClass<PSYSTEM_MODULE_INFORMATION> {
    static constexpr auto value = SystemModuleInformation;
  };

  template <typename SystemInformation>
  SystemInformation get_system_informations()
  {
    auto NtQuerySystemInformation = (_NtQuerySystemInformation)::ul::get_module_export("ntdll.dll", "NtQuerySystemInformation");
    static_assert(SystemInformationClass<SystemInformation>::value, "Missing SystemInformationClass specialization");
    auto system_information_class = SystemInformationClass<SystemInformation>::value;
    SystemInformation system_informations = NULL;
    ULONG length = 0x1000;
    for (;;) {
      system_informations = (SystemInformation)VirtualAlloc(NULL, length, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
      if (system_informations == NULL) break;

      auto status = NtQuerySystemInformation(system_information_class, system_informations, length, &length);
      if (status != STATUS_INFO_LENGTH_MISMATCH) break;

      if (system_informations != NULL) VirtualFree(system_informations, 0, MEM_RELEASE);

      length <<= 2;
    }

    return system_informations;
  }
}  // namespace ul
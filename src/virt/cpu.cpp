#include "virt/cpu.h"

#include <iostream>

extern "C" void asm_get_cpu_vendor(char *);
extern "C" void asm_get_cpu_brand(char *);

namespace ul
{
  std::string get_cpu_vendor()
  {
    std::string vendor(13, 0);
    asm_get_cpu_vendor(vendor.data());
    return vendor;
  }

  std::string get_cpu_brand()
  {
    std::string vendor(48, 0);
    asm_get_cpu_brand(vendor.data());
    return vendor;
  }
}  // namespace ul
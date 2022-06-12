#include "virt/vmx.h"

extern "C" bool asm_has_vmx(void);

namespace ul
{
  bool has_vmx(void) { return asm_has_vmx(); }
}  // namespace ul
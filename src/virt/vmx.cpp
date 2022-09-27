#include "virt/vmx.h"

extern "C" bool asm_has_vmx_bit(void);

namespace ul
{
  bool has_vmx_bit(void) { return asm_has_vmx_bit(); }
}  // namespace ul
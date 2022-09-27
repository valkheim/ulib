section .text
bits 64

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global asm_has_vmx_bit
asm_has_vmx_bit:
    mov eax, 1
    cpuid

    bt ecx, 0x5
    jc asm_has_vmx_ok

    xor rax, rax
    ret

asm_has_vmx_ok:
    mov rax, 0x1
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
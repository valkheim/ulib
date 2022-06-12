section .text
bits 64

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global asm_get_cpu_vendor
asm_get_cpu_vendor:
	; Store first argument
	mov r8, rcx

	; Execute CPUID with EAX = 0 to get the CPU producer
	xor eax, eax
	cpuid

	; Copy CPU vendor
	mov [r8 + 0], ebx		; Genu
	mov [r8 + 4], edx		; ineI
	mov [r8 + 8], ecx		; ntel
	mov byte [r8 + 12], 0 	; NUL

	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global asm_get_cpu_brand
asm_get_cpu_brand:
	; Store first argument
	mov r8, rcx

	; Initial value for cpuid
	mov r15d, 0x80000002

	; Retrieve the 47 bytes of the CPU brand 
asm_get_cpu_brand_loop:
	mov eax, r15d
	cpuid

	mov [r8 + 0x0], eax
	mov [r8 + 0x4], ebx
	mov [r8 + 0x8], ecx
	mov [r8 + 0xc], edx

	add r8, 0x10
	inc r15d
	cmp r15d, 0x80000004
	jle asm_get_cpu_brand_loop

	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

.include "src/p/x86_64/x86_64.inc"

# int hebi_cpuid__(struct hebi_cpuidregs *regs, uint32_t leaf, uint32_t subleaf);

FUNC_BEGIN cpuid, @private

    # save rbx and place subleaf in ebp

    push        %rbp
    push        %rbx
    mov         %edx, %ebp

    # make sure leaf is supported by checking highest function parameter

    mov         %esi, %eax
    xor         %ecx, %ecx
    and         $0x80000000, %eax
    cpuid
    cmp         %esi, %eax
    jl          1f

    # query information for the leaf/subleaf and return true

    mov         %ebp, %ecx
    mov         %esi, %eax
    cpuid
    mov         %eax, (%rdi)
    mov         %ebx, 4(%rdi)
    mov         %ecx, 8(%rdi)
    mov         %edx, 12(%rdi)
    mov         $1, %eax
    pop         %rbx
    pop         %rbp
    ret

    # clear output on failure and return false

    .p2align 4,,7
1:  xor         %eax, %eax
    mov         %eax, (%rdi)
    mov         %eax, 4(%rdi)
    mov         %eax, 8(%rdi)
    mov         %eax, 12(%rdi)
    pop         %rbx
    pop         %rbp
    ret

FUNC_END

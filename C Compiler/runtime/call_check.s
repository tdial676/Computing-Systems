# This is a custom entry point that calls basic_main() for a compiled BASIC program
# and verifies that it satisfies the x86-64 calling convention.
# This means that the callee-save registers and all memory above %rsp
# at the start of basic_main() must not be modified.

.rodata
check_failed_msg:
    .string "x86-64 calling convention violated\n"

.text
.globl _start
_start:
    # Put 32 KB of canary data on the stack
    movq $-0x1000, %rcx
    leaq (%rsp, %rcx, 8), %rsp
    movq %rsp, %rdi
    movq $0x1BF532C94A760E8D, %rax
    negq %rcx
    cld
    rep stosq

    # Put canary values in callee-save registers
    movq $0xB42607DE9FA358C1, %rbx
    movq $0x412087E6FAB9C53D, %rbp
    movq $0x514D387C6AE02B9F, %r12
    movq $0xE5BF4238C1DA9760, %r13
    movq $0xCB9FD8524A630E71, %r14
    movq $0xD0B5F947CEA23618, %r15

    call basic_main

    # Check that callee-save registers haven't changed
    movq $0xB42607DE9FA358C1, %rax
    cmpq %rax, %rbx
    jne check_failed
    movq $0x412087E6FAB9C53D, %rax
    cmpq %rax, %rbp
    jne check_failed
    movq $0x514D387C6AE02B9F, %rax
    cmpq %rax, %r12
    jne check_failed
    movq $0xE5BF4238C1DA9760, %rax
    cmpq %rax, %r13
    jne check_failed
    movq $0xCB9FD8524A630E71, %rax
    cmpq %rax, %r14
    jne check_failed
    movq $0xD0B5F947CEA23618, %rax
    cmpq %rax, %r15
    jne check_failed

    # Check canary values on stack
    movq %rsp, %rdi
    movq $0x1BF532C94A760E8D, %rax
    movl $0x1000, %ecx
    repe scasq
    jne check_failed

    # Checks succeeded; exit with code 0
    movl $0, %edi
    call exit@plt

    # Checks failed; print a warning message and exit with error status
    check_failed:
    leaq check_failed_msg(%rip), %rdi
    movq stderr(%rip), %rsi
    call fputs@plt
    movl $1, %edi
    call exit@plt

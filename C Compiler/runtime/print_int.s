.rodata
int_format:
    .string "%ld\n"

.text
.globl print_int
print_int:
    movq %rdi, %rsi
    leaq int_format(%rip), %rdi # LABEL(%rip) computes the address of LABEL relative to %rip
    movb $0, %al # %al stores the number of float arguments to printf()
    call printf@plt

    # Clobber all caller-save registers
    movq $0x6A2CFE91073BD845, %rax
    movq $0x03BAD7C14F2E6589, %rdi
    movq $0x5D41EA960C72F8B3, %rsi
    movq $0xEC364B2D5A7F9810, %rdx
    movq $0xFC85AD49320BE167, %rcx
    movq $0x529A48CDB7163E0F, %r8
    movq $0x92E1587A4BDCF630, %r9
    movq $0x47DC36501F89AEB2, %r10
    movq $0xAF4B29785C61ED30, %r11

    ret

.code16
.globl start
start:
    movb $0x0e, %ah
    movb hello, %al  # what is moved into %al ?  w
    movb $0,    %bh
    movb $7,    %bl
    int $0x10

stop:
    jmp stop

hello:
    .string "Welcome to xv6@UIC. Press any key to continue"

.org 0x1fe
.word 0xAA55

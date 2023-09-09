.code16
.globl start
start:
    jmp printString

printString:
    movb $0x0e, %ah
    movb $7,    %bl
    movb $0,    %bh

    movw $hello, %si
    jmp printLoop

printLoop:
    movb (%si), %al    # sets al to a character from hello    
    incw %si    # increments si to next character

    cmpb $0, %al     # check if end of string
    je waitForKey

    int $0x10   # call interrupt
    jmp printLoop       # loop back


waitForKey:
    movb $0x00, %ah
    int $0x16

    jmp printPicture

printPicture:
    movb $0x13, %al
    movb $0x00, %ah

    movb $0x12, 0xA0000

    int $0x10

    jmp stop

stop:
    jmp stop

hello:
    .string "Welcome to xv6@UIC. Press any key to continue"

.org 0x1fe
.word 0xAA55

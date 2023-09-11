.code16
.globl start
start:
    movb %dl, %cl   # saving drive number for later
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
    int $0x16 # waits for key press

    jmp videoMode

videoMode:
    movb $0x13, %al
    movb $0x00, %ah

    int $0x10 # switches to video mode

    movw $0xA000, %ax 
    movw %ax, %ds  # In i8086 you cannot move imm directly to a *s register. (movw %0xA000, %ds)
    movw $0, %di   # The first pixel at the top-left corner of the screen
    movb $0xf, %al # white pixel
    movb %al, %ds:(%di) # After this instruction you can see the tiny white dot there.
    
    jmp load

load:
    xor %ax, %ax
    movw %ax, %ds             # Sets ds to 0 and offset to memory location so
    movw $packet, %si         # DS*16 + SI = packet location

    movb $0x42, %ah           # for subfunction
    movb %cl, %dl             # drive index
    int $0x13                 # calls interruption

    jmp stop

stop:
    jmp stop

hello:
    .string "Welcome to xv6@UIC. Press any key to continue"


packet:
    .byte 0x10, 0x00 # must be this
    .word 0x007d # number of 512 byte groups needed to load for image 
    .word 0x0000, 0xA000 # video memory
    .quad 0x01 # needs to be 1 because first sector is code above

.org 0x1fe
.word 0xAA55

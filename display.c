#include <stdarg.h>

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "vga.h"

static uchar* displayBuf = (uchar*)P2V(0xA0000);

#define CRTPORT 0x3d4
int pos = 0;
static ushort* crt = (ushort*)P2V(0xb8000);
int* buffer[24*80];

void
saveBuffer() {
    memmove(buffer, crt, sizeof(crt[0])*25*80);
    outb(CRTPORT, 14);
    pos = inb(CRTPORT+1) << 8;
    outb(CRTPORT, 15);
    pos |= inb(CRTPORT+1);
}

void
restoreBuffer(struct file* f) {
    memmove(crt, buffer, sizeof(crt[0])*25*80);
    outb(CRTPORT, 14);
    outb(CRTPORT+1, pos>>8);
    outb(CRTPORT, 15);
    outb(CRTPORT+1, pos);
    crt[pos] = ' ' | 0x0700;
}

void
displayputc(int c, struct file* f) {
    displayBuf[f->off++] = (c & 0xff);
}

int
displayioctl(struct file *f, int param, int value) {
    if(param == 1) {
        if(value == 0x13) {
            // save data here
            saveBuffer();
            vgaMode13();
            f->off = 0;
        } else if(value == 0x3) {
            vgaMode3();
            // restore data here
            restoreBuffer(f);
        } else {
            return -1;
        }
        return 1;
    } else if (param == 2) {
        int index = (value >> 24) & 0xff;
        int red = (value >> 16) & 0xff; 
        int green = (value >> 8) & 0xff; 
        int blue = (value >> 0) & 0xff;
        vgaSetPalette(index, red, green, blue);
        
        return 1;
    }
    return -1;
}

int
displaywrite(struct file *f, char *buf, int n)
{
    int i;
    for(i = 0; i < n; i++) {
        displayputc(buf[i] & 0xff, f); // Write a pixel
    }
    return n;
}

void
displayinit(void) {
    devsw[DISPLAY].write = displaywrite;
}
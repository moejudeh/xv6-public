#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

addr_t
sys_sbrk(void)
{
  addr_t addr;
  addr_t n;

  argaddr(0, &n);
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

#define MMAP_FAILED ((~0lu))
  addr_t
sys_mmap(void)
{
  int fd, flags;
  if(argint(0,&fd) < 0 || argint(1,&flags) < 0)
    return MMAP_FAILED;
   
  if(flags == 0) { 
    struct file* f = proc->ofile[fd];
    
    // Check if the file descriptor is valid
    if (f == 0) {
        return MMAP_FAILED;
    }

    // Get the size of the file
    int size = f->ip->size;

    // Update process metadata to keep track of the mapping
    proc->mmaps[proc->mmapcount].fd = fd;
    proc->mmaps[proc->mmapcount].start = proc->mmaptop;
    proc->mmapcount++;
    proc->mmaptop += PGSIZE;

    // Allocate memory for the process
    char* mapped_memory = (char*)kalloc();
    
    if (mapped_memory == 0) {
        return MMAP_FAILED; // Handle allocation failure
    }

    // Read the entire file's data into the allocated memory
    int bytesRead = readi(f->ip, mapped_memory, f->off, size);

    // Map the allocated memory to the desired address range
    mappages(proc->pgdir, (void*)proc->mmaptop - PGSIZE, PGSIZE, v2p(mapped_memory), PTE_W | PTE_U);

    return (void*)(proc->mmaptop - (PGSIZE));
  }

  return MMAP_FAILED;
}

  int
handle_pagefault(addr_t va)
{
  // TODO: your code here
  return 0;
}

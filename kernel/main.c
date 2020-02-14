#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("1\n");
    kinit();         // physical page allocator
    printf("2\n");
    kvminit();       // create kernel page table
    printf("3\n");
    kvminithart();   // turn on paging
    printf("4\n");
    procinit();      // process table
    printf("5\n");
    trapinit();      // trap vectors
    printf("6\n");
    trapinithart();  // install kernel trap vector
    printf("7\n");
    plicinit();      // set up interrupt controller
    printf("8\n");
    plicinithart();  // ask PLIC for device interrupts
    printf("9\n");
    binit();         // buffer cache
    printf("10\n");
    iinit();         // inode cache
    printf("11\n");
    fileinit();      // file table
    printf("12\n");
    virtio_disk_init(); // emulated hard disk
    printf("13\n");
    userinit();      // first user process
    printf("14\n");
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  scheduler();        
}

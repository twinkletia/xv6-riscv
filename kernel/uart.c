//
// low-level driver routines for 16550a UART.
//

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#define Reg(reg) ((volatile unsigned char *)(UART0 + reg))

// the UART control registers.
// some have different meanings for
// read vs write.
// http://byterunner.com/16550.html
#define RHR 0  // receive holding register (for input bytes)
#define THR 0  // transmit holding register (for output bytes)
#define IER 4  // interrupt enable register
#define FCR 8  // FIFO control register
#define ISR 8  // interrupt status register
#define LCR 12 // line control register
#define LSR 20 // line status register

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

#define UART_TX_BUF ((volatile unsigned int *)0x40000000)
#define UART_TX_STAT ((volatile unsigned int *)0x40000004)

#define UART_RX_BUF ((volatile unsigned int *)0x40000010)
#define UART_RX_STAT ((volatile unsigned int *)0x40000014)

#define UART_TX_GET_STAT_FULL() (((*(UART_TX_STAT)) & 0x00000008) >> 3)
#define UART_TX_GET_STAT_EMPTY() (((*(UART_TX_STAT)) & 0x00000004) >> 2)
#define UART_TX_GET_STAT_BUSY() (((*(UART_TX_STAT)) & 0x00000002) >> 1)
#define UART_TX_GET_STAT_EN() (((*(UART_TX_STAT)) & 0x00000001))
#define UART_TX_SET_EN(en) (*(UART_TX_STAT) = *(UART_TX_STAT) | ((en)&0x1));

#define UART_RX_GET_STAT_FULL() (((*(UART_RX_STAT)) & 0x00000008) >> 3)
#define UART_RX_GET_STAT_EMPTY() (((*(UART_RX_STAT)) & 0x00000004) >> 2)
#define UART_RX_GET_STAT_BUSY() (((*(UART_RX_STAT)) & 0x00000002) >> 1)
#define UART_RX_GET_STAT_EN() (((*(UART_RX_STAT)) & 0x00000001))
#define UART_RX_SET_EN(en) (*(UART_RX_STAT) = *(UART_RX_STAT) | ((en)&0x1));

typedef union
{
  struct
  {
    unsigned int en : 1;
    unsigned int busy : 1;
    unsigned int empty : 1;
    unsigned int full : 1;
    unsigned int unused : 28;
  } stat;
  unsigned int val;
} uart_tx_stat_t;

typedef uart_tx_stat_t uart_rx_stat_t;

int uart_putchar(int ch);
int uart_getchar(void);

extern __attribute__((weak)) void uart_rx_interrupt_handler(void);
extern __attribute__((weak)) void uart_tx_interrupt_handler(void);

void uartinit(void)
{
  /*
  // disable interrupts.
  WriteReg(IER, 0x00);

  // special mode to set baud rate.
  WriteReg(LCR, 0x80);

  // LSB for baud rate
  //WriteReg(0, 0xf9);
  // MSB for baud rate
  //WriteReg(4, 0x15);

  // leave set-baud mode,
  // and set word length to 8 bits, no parity.
  WriteReg(LCR, 0x03);

  // reset and enable FIFOs.
  WriteReg(FCR, 0x07);

  // enable receive interrupts.
  WriteReg(IER, 0x01);
  */
  UART_RX_SET_EN(1);
}

// write one output character to the UART.
void uartputc(int c)
{
  // wait for Transmit Holding Empty to be set in LSR.
  /*
  while((ReadReg(LSR) & (1 << 5)) == 0)
    ;
  WriteReg(THR, c);
  */
  while (UART_TX_GET_STAT_FULL())
    asm volatile("nop");

  *UART_TX_BUF = c;
  UART_TX_SET_EN(1);
}

// read one input character from the UART.
// return -1 if none is waiting.
int uartgetc(void)
{
  int ch;

  // UART_RX_SET_EN(1);

  /* Blocking */
  /*
  while(UART_RX_GET_STAT_EMPTY()) {
    asm volatile("nop");
  }*/
  ch = *UART_RX_BUF;
  if (ch)
  {
    return ch;
  }
  else
  {
    return -1;
  }
}

// trap.c calls here when the uart interrupts.
void uartintr(void)
{
  while (1)
  {
    int c = uartgetc();
    if (c == -1)
      break;
    consoleintr(c);
  }
}

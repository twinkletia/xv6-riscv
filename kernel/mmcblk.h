#ifndef MMCBLK_H
#define MMCBLK_H

#define MMC_BASE 0x40001000
#define MMC_STATUS ((volatile unsigned int *) (MMC_BASE + 0x200))
#define MMC_ADRS ((volatile unsigned int *) (MMC_BASE + 0x214))
#define MMC_OP ((volatile unsigned int *) (MMC_BASE + 0x210))
#define MMC_DATA_BASE ((volatile unsigned int *) (MMC_BASE + 0x0))

#endif


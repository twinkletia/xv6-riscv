#ifndef MMCBLK_H
#define MMCBLK_H

#define SD_BASE 0x40001000
#define SD_STATUS ((volatile unsigned int *) (SD_BASE + 0x200))
#define SD_ADRS ((volatile unsigned int *) (SD_BASE + 0x214))
#define SD_OP ((volatile unsigned int *) (SD_BASE + 0x210))
#define SD_DATA_BASE ((volatile unsigned int *) (SD_BASE + 0x0))

#endif


#ifndef MMCBLK_H
#define MMCBLK_H

#define MMC_BASE 0x40001000
#define MMC_STATUS ((volatile unsigned int *)(MMC_BASE + 0x200))
#define MMC_R1STAT ((volatile unsigned int *)(MMC_BASE + 0x204))
#define MMC_RESP ((volatile unsigned int *)(MMC_BASE + 0x208))
#define MMC_CMD ((volatile unsigned int *)(MMC_BASE + 0x20C))
#define MMC_ARG ((volatile unsigned int *)(MMC_BASE + 0x210))
#define MMC_DATA_BASE ((volatile unsigned int *)(MMC_BASE + 0x0))
#define INITED 0x1
#define IDLE 0x2
#define EXEC 0x4
#define INTR_EN 0x8
#define OK (INITED | IDLE)

#endif

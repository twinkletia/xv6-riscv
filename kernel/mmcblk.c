#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"
#include "mmcblk.h"

struct spinlock mmcblk_lock;
char *mmcspi_chan;

void mmcblk_init(void)
{
	initlock(&mmcblk_lock, "mmcblk");

	while ((*MMC_STATUS) != OK)
	{
		asm volatile("nop");
	}
	*MMC_STATUS = (*MMC_STATUS) | INTR_EN;
}
void mmcblk_read(struct buf *b)
{
	uint32 sector = b->blockno * BSIZE;

	acquire(&mmcblk_lock);
	// printf("mmcblk_read: b->blockno:%d, sector:%d\n", b->blockno, sector);

	*MMC_CMD = 17;
	*MMC_ARG = sector;
	*MMC_STATUS = *MMC_STATUS | EXEC;

	__sync_synchronize();

	sleep(&mmcspi_chan, &mmcblk_lock);

	for (int i = 0; i < BSIZE / 2; i += 4)
	{
		union
		{
			unsigned int v;
			unsigned char x[4];
		} data;

		data.v = MMC_DATA_BASE[0 + (i >> 2)];

		b->data[i + 0] = data.x[0];
		b->data[i + 1] = data.x[1];
		b->data[i + 2] = data.x[2];
		b->data[i + 3] = data.x[3];
	}

	*MMC_CMD = 17;
	*MMC_ARG = sector + 512;
	*MMC_STATUS = *MMC_STATUS | EXEC;

	__sync_synchronize();

	sleep(&mmcspi_chan, &mmcblk_lock);

	for (int i = 512; i < BSIZE; i += 4)
	{
		union
		{
			unsigned int v;
			unsigned char x[4];
		} data;

		data.v = MMC_DATA_BASE[0 + ((i - 512) >> 2)];

		b->data[i + 0] = data.x[0];
		b->data[i + 1] = data.x[1];
		b->data[i + 2] = data.x[2];
		b->data[i + 3] = data.x[3];
	}

	/*
	for(int i = 0; i < BSIZE; i++) {
		printf("%02x\n", b->data[i]);
	}
	*/

	release(&mmcblk_lock);
}

void mmcblk_write(struct buf *b)
{
	uint32 sector = b->blockno * BSIZE;

	acquire(&mmcblk_lock);
	// printf("mmcblk_write: b->blockno:%d, sector:%d\n", b->blockno, sector);

	for (int i = 0; i < BSIZE / 2; i += 4)
	{
		union
		{
			unsigned int v;
			unsigned char x[4];
		} data;

		data.x[0] = b->data[i + 0];
		data.x[1] = b->data[i + 1];
		data.x[2] = b->data[i + 2];
		data.x[3] = b->data[i + 3];

		MMC_DATA_BASE[0 + (i >> 2)] = data.v;
	}

	*MMC_CMD = 24;
	*MMC_ARG = sector;
	*MMC_STATUS = *MMC_STATUS | EXEC;

	sleep(&mmcspi_chan, &mmcblk_lock);
	// panic("mmcblk_write");

	for (int i = 512; i < BSIZE; i += 4)
	{
		union
		{
			unsigned int v;
			unsigned char x[4];
		} data;

		data.x[0] = b->data[i + 0];
		data.x[1] = b->data[i + 1];
		data.x[2] = b->data[i + 2];
		data.x[3] = b->data[i + 3];

		MMC_DATA_BASE[0 + ((i - 512) >> 2)] = data.v;
	}

	*MMC_CMD = 24;
	*MMC_ARG = sector + 512;
	*MMC_STATUS = *MMC_STATUS | EXEC;

	sleep(&mmcspi_chan, &mmcblk_lock);
	__sync_synchronize();

	release(&mmcblk_lock);
}
void mmcblk_intr(void)
{
	acquire(&mmcblk_lock);
	// panic("mmcblk_intr");
	wakeup(&mmcspi_chan);
	release(&mmcblk_lock);
}

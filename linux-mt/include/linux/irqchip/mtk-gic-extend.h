/*
 * Copyright (c) 2014 MediaTek Inc.
 * Author: Maoguang.Meng <maoguang.meng@mediatek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __MTK_GIC_EXTEND_H
#define __MTK_GIC_EXTEND_H

#define MT_EDGE_SENSITIVE	0
#define MT_LEVEL_SENSITIVE	1
#define MT_POLARITY_LOW		0
#define MT_POLARITY_HIGH	1

#ifndef FIQ_SMP_CALL_SGI
#define FIQ_SMP_CALL_SGI	13
#endif

typedef void (*fiq_isr_handler) (void *arg, void *regs, void *svc_sp);

enum {
	IRQ_MASK_HEADER = 0xF1F1F1F1,
	IRQ_MASK_FOOTER = 0xF2F2F2F2
};

struct mtk_irq_mask {
	unsigned int header;	/* for error checking */
	__u32 mask0;
	__u32 mask1;
	__u32 mask2;
	__u32 mask3;
	__u32 mask4;
	__u32 mask5;
	__u32 mask6;
	__u32 mask7;
	__u32 mask8;
	__u32 mask9;
	__u32 mask10;
	__u32 mask11;
	__u32 mask12;
	unsigned int footer;	/* for error checking */
};

unsigned int get_hardware_irq(unsigned int virq);
#if defined(CONFIG_FIQ_GLUE)
int request_fiq(int irq, fiq_isr_handler handler, unsigned long irq_flags, void *arg);
void irq_raise_softirq(const struct cpumask *mask, unsigned int irq);
#endif

#endif


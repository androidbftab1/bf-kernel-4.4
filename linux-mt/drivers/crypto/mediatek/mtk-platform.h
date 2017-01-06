/*
 * Support for MediaTek cryptographic accelerator.
 *
 * Copyright (c) 2016 MediaTek Inc.
 * Author: Ryder Lee <ryder.lee@mediatek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 */

#ifndef __MTK_PLATFORM_H_
#define __MTK_PLATFORM_H_

#include <linux/crypto.h>
#include <crypto/internal/hash.h>
#include <linux/interrupt.h>

#define MTK_RDR_THRESH_DEF	0x800001

#define MTK_IRQ_RDR0		BIT(1)
#define MTK_IRQ_RDR1		BIT(3)
#define MTK_IRQ_RDR2		BIT(5)
#define MTK_IRQ_RDR3		BIT(7)

#define MTK_DESC_CNT_CLR	BIT(31)
#define MTK_DESC_LAST		BIT(22)
#define MTK_DESC_FIRST		BIT(23)
#define MTK_DESC_BUF_LEN(x)	((x) & 0x1ffff)
#define MTK_DESC_CT_LEN(x)	(((x) & 0xff) << 24)

#define WORD(x)			((x) >> 2)

/**
 * Ring 0/1 are used by AES encrypt and decrypt.
 * Ring 2/3 are used by SHA.
 */
enum {
	RING0 = 0,
	RING1,
	RING2,
	RING3,
	RING_MAX,
};

#define RECORD_NUM		(RING_MAX / 2)

/**
 * struct mtk_desc - DMA descriptor
 * @hdr:	the descriptor control header
 * @buf:	DMA address of input buffer
 * @ct:		the command token that control operation flow
 * @ct_hdr:	the command token control header
 * @tag:	the user-defined field
 * @tfm:	DMA address of transform state
 * @bound:	align descriptors offset boundary
 *
 * Structure passed to the crypto engine to describe the crypto
 * operation to be executed.
 */
struct mtk_desc {
	u32 hdr;
	u32 buf;
	u32 ct;
	u32 ct_hdr;
	u32 tag;
	u32 tfm;
	u32 bound[2];
};

/**
 * struct mtk_ring - Descriptor ring
 * @cmd_base:	pointer to command descriptor ring base
 * @cmd_dma:	DMA address of command descriptor ring
 * @res_base:	pointer to result descriptor ring base
 * @res_dma:	DMA address of result descriptor ring
 * @pos:	current position in the ring
 */
struct mtk_ring {
	struct mtk_desc *cmd_base;
	dma_addr_t cmd_dma;
	struct mtk_desc *res_base;
	dma_addr_t res_dma;
	u32 pos;
};

#define MTK_MAX_DESC_NUM	512
#define MTK_DESC_OFFSET		WORD(sizeof(struct mtk_desc))
#define MTK_DESC_SIZE		(MTK_DESC_OFFSET - 2)
#define MTK_MAX_RING_SIZE	((sizeof(struct mtk_desc) * MTK_MAX_DESC_NUM))
#define MTK_DESC_CNT(x)		((MTK_DESC_OFFSET * (x)) << 2)

/**
 * struct mtk_aes_dma - Structure that holds sg list info
 * @sg:		pointer to scatter-gather list
 * @nents:	number of entries in the sg list
 * @remainder:	remainder of sg list
 * @sg_len:	number of entries in the sg mapped list
 */
struct mtk_aes_dma {
	struct scatterlist *sg;
	int nents;
	u32 remainder;
	u32 sg_len;
};

/**
 * struct mtk_aes - AES operation record
 * @queue:	crypto request queue
 * @req:	pointer to ablkcipher request
 * @task:	the tasklet is use in AES interrupt
 * @src:	the structure that holds source sg list info
 * @dst:	the structure that holds destination sg list info
 * @aligned_sg:	the scatter list is use to alignment
 * @real_dst:	pointer to the destination sg list
 * @total:	request buffer length
 * @buf:	pointer to page buffer
 * @info:	pointer to AES transform state and command token
 * @ct_hdr:	AES command token control field
 * @ct_size:	size of AES command token
 * @ct_dma:	DMA address of AES command token
 * @tfm_dma:	DMA address of AES transform state
 * @id:		record identification
 * @flags:	it's describing AES operation state
 * @lock:	the ablkcipher queue lock
 *
 * Structure used to record AES execution state
 */
struct mtk_aes {
	struct crypto_queue queue;
	struct ablkcipher_request *req;
	struct tasklet_struct task;
	struct mtk_aes_dma src;
	struct mtk_aes_dma dst;

	struct scatterlist aligned_sg;
	struct scatterlist *real_dst;

	size_t total;
	void *buf;

	void *info;
	u32 ct_hdr;
	u32 ct_size;
	dma_addr_t ct_dma;
	dma_addr_t tfm_dma;

	u8 id;
	unsigned long flags;
	/* queue lock */
	spinlock_t lock;
};

/**
 * struct mtk_sha - SHA operation record
 * @queue:	crypto request queue
 * @req:	pointer to ahash request
 * @task:	the tasklet is use in SHA interrupt
 * @info:	pointer to SHA transform state and command token
 * @ct_hdr:	SHA command token control field
 * @ct_size:	size of SHA command token
 * @ct_dma:	DMA address of SHA command token
 * @tfm_dma:	DMA address of SHA transform state
 * @id:		record identification
 * @flags:	it's describing SHA operation state
 * @lock:	the ablkcipher queue lock
 *
 * Structure used to record SHA execution state.
 */
struct mtk_sha {
	struct crypto_queue queue;
	struct ahash_request *req;
	struct tasklet_struct task;

	void *info;
	u32 ct_hdr;
	u32 ct_size;
	dma_addr_t ct_dma;
	dma_addr_t tfm_dma;

	u8 id;
	unsigned long flags;
	/* queue lock */
	spinlock_t lock;
};

/**
 * struct mtk_cryp - Cryptographic device
 * @base:	pointer to mapped register I/O base
 * @dev:	pointer to device
 * @clk_ethif:	pointer to ethif clock
 * @clk_cryp:	pointer to crypto clock
 * @irq:	global system and rings IRQ
 * @ring:	pointer to execution state of AES
 * @aes:	pointer to execution state of SHA
 * @sha:	each execution record map to a ring
 * @aes_list:	device list of AES
 * @sha_list:	device list of SHA
 * @tmp:	pointer to temporary buffer for internal use
 * @tmp_dma:	DMA address of temporary buffer
 * @rec:	it's used to select SHA record for tfm
 *
 * Structure storing cryptographic device information.
 */
struct mtk_cryp {
	void __iomem *base;
	struct device *dev;
	struct clk *clk_ethif;
	struct clk *clk_cryp;
	int irq[5];

	struct mtk_ring *ring[RING_MAX];
	struct mtk_aes *aes[RECORD_NUM];
	struct mtk_sha *sha[RECORD_NUM];

	struct list_head aes_list;
	struct list_head sha_list;

	void *tmp;
	dma_addr_t tmp_dma;
	bool rec;
};

int mtk_cipher_alg_register(struct mtk_cryp *cryp);
void mtk_cipher_alg_release(struct mtk_cryp *cryp);
int mtk_hash_alg_register(struct mtk_cryp *cryp);
void mtk_hash_alg_release(struct mtk_cryp *cryp);

#endif /* __MTK_PLATFORM_H_ */

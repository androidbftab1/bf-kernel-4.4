/*
 * Mediatek AHCI SATA driver.
 *
 * Author: Long Cheng <long.cheng@mediatek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pm.h>
#include <linux/device.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/libata.h>
#include <linux/ahci_platform.h>
#include <linux/pci_ids.h>
#include "ahci.h"

#define DRV_NAME "ahci-mtk"

#define MTK_DIAGNR 0xB4
#define MTK_PxTFD 0x120
#define MTK_PxSIG 0x124
#define MTK_PxSSTS 0x128

#define	BIT_IE_PSE (1 << 1)
#define	BIT_IE_DHRE (1 << 0)

/* others */
#define DEASSERT_RESET_ADDRESS 0xF005A2E0
#define GCSR_PHYDONE_DC 0xDC
#define PHYDONE_PHYDONE_MASK 0x00000001

/* for PxFB */
static void *fis_mem;
static dma_addr_t mem_dma;
static size_t dma_sz = 256;

static const struct ata_port_info ahci_port_info = {
	.flags		= AHCI_FLAG_COMMON,
	.pio_mask	= ATA_PIO4,
	.udma_mask	= ATA_UDMA6,
	.port_ops	= &ahci_platform_ops,
};

static struct scsi_host_template ahci_platform_sht = {
	AHCI_SHT(DRV_NAME),
};

static int host_init(struct device *dev, void __iomem *mmio)
{
	unsigned int count = 2000;
	void __iomem *deassert_reset_addr;

	if (!mmio) {
		dev_err(dev, "[mtahci]:%s fail @line%d\n", __func__, __LINE__);
		return -1;
	}

	deassert_reset_addr = ioremap(DEASSERT_RESET_ADDRESS, 0x04);

	dev_info(dev, "[mtahci] init: waiting for PHY done...\n");
	while ((!(PHYDONE_PHYDONE_MASK & readl(mmio + GCSR_PHYDONE_DC))) && (count-- != 0))
		udelay(1000);

	if (count > 0) {
		dev_info(dev, "[mtahci] init: PHY is done!\n");
	} else {
		dev_err(dev, "[mtahci] init error: waiting for PHY done timeout!\n");
		return -1;
	}

	writel(0x80000000 | (~0x80000000 & readl(deassert_reset_addr)), deassert_reset_addr);
	writel(0x00000000 | (~0x80000000 & readl(deassert_reset_addr)), deassert_reset_addr);
	iounmap(deassert_reset_addr);
	dev_info(dev, "[mtahci] init: deassert top soft reset!\n");

	return 0;
}

static int hba_init(struct device *dev, void __iomem *mmio)
{
	unsigned int regvalue = 0;
	int count = 500;

	if (!mmio) {
		dev_err(dev, "[mtahci]:%s fail @line%d\n", __func__, __LINE__);
		return -1;
	}

	/* Set Host Hwinit registers */
	if (host_init(dev, mmio) < 0) {
		dev_err(dev, "[mtahci] init: host init failed!\n");
		return -1;
	}

	udelay(1000);

	/* CAP */
	regvalue = readl(mmio+HOST_CAP);
	regvalue &= (~(HOST_CAP_SSS | HOST_CAP_MPS | HOST_CAP_SXS));
	writel(regvalue, mmio+HOST_CAP);

	/* PI */
	regvalue = 0x00000001;
	writel(regvalue, mmio+HOST_PORTS_IMPL);

	dev_info(dev, "[mtahci]:before GHC\n");
	dev_info(dev, "[mtahci]:*0x%08lx = 0x%08x\n", (unsigned long)(mmio + 0xb4), readl(mmio + MTK_DIAGNR));
	dev_info(dev, "[mtahci]:*0x%08lx = 0x%08x\n", (unsigned long)(mmio + 0xb4), readl(mmio + MTK_PxTFD));
	dev_info(dev, "[mtahci]:*0x%08lx = 0x%08x\n", (unsigned long)(mmio + 0xb4), readl(mmio + MTK_PxSIG));
	dev_info(dev, "[mtahci]:*0x%08lx = 0x%08x\n", (unsigned long)(mmio + 0xb4), readl(mmio + MTK_PxSSTS));

	/* TODO: TESTR need Do?  */

	/* TODO: PxVS1 need Do?  */

	/* GHC  */
	regvalue = 0x80000001;
	writel(regvalue, mmio+HOST_CTL);

	while ((readl(mmio + HOST_CTL) == HOST_RESET) && (count > 0)) {
		count--;
		udelay(1000);
	}

	return 0;
}

static int port_init(struct device *dev, void __iomem *mmio, unsigned int port_no)
{
	unsigned int count = 2000;
	unsigned int regvalue = 0;

	/* PxCMD  */
	regvalue = readl(mmio + 0x100 + (port_no * 0x80) + PORT_SCR_CTL);
	regvalue &= ~0x0F;
	writel(regvalue, (mmio + 0x100 + (port_no * 0x80) + PORT_SCR_CTL));

	fis_mem = dmam_alloc_coherent(dev, dma_sz, &mem_dma, GFP_KERNEL);
	if (!fis_mem)
		return -ENOMEM;
	memset(fis_mem, 0, dma_sz);

	/* PxFB  */
	writel(mem_dma, (mmio + 0x100 + (port_no * 0x80) + PORT_FIS_ADDR));

	/* PxCMD  */
	regvalue = readl(mmio + 0x100 + (port_no * 0x80) + PORT_CMD);
	regvalue |= PORT_CMD_FIS_RX;
	writel(regvalue, (mmio + 0x100 + (port_no * 0x80) + PORT_CMD));

	dev_info(dev, "[mtahci]:after GHC\n");
	dev_info(dev, "[mtahci]:*0x%08lx = 0x%08x\n", (unsigned long)(mmio + 0xb4), readl(mmio+0xb4));
	dev_info(dev, "[mtahci]:*0x%08lx = 0x%08x\n", (unsigned long)(mmio + 0x120), readl(mmio+0x120));
	dev_info(dev, "[mtahci]:*0x%08lx = 0x%08x\n", (unsigned long)(mmio + 0x124), readl(mmio+0x124));
	dev_info(dev, "[mtahci]:*0x%08lx = 0x%08x\n", (unsigned long)(mmio + 0x128), readl(mmio+0x128));

	dev_info(dev, "[mtahci] init: waiting for ssts!\n");
	do {
		regvalue = readl(mmio + 0x100 + (port_no * 0x80) + PORT_SCR_STAT);
		udelay(1000);
	} while (((regvalue & 0xF) != 3) && (count-- != 0));

	if (count > 0) {
		dev_info(dev, "[mtahci] init: link ready!\n");
	} else {
		dev_err(dev, "[mtahci] init error: link timeout!\n");
		return -1;
	}

	/* clear serr  */
	writel(0xFFFFFFFF, (mmio + 0x100 + (port_no * 0x80) + PORT_SCR_ERR));
	/* clear port intr  */
	writel(0xFFFFFFFF, (mmio + 0x100 + (port_no * 0x80) + PORT_IRQ_STAT));

	regvalue = (BIT_IE_PSE | BIT_IE_DHRE);
	writel(regvalue, (mmio + 0x100 + (port_no * 0x80) + PORT_IRQ_MASK));

	/* clear global intr  */
	writel(0xFFFFFFFF, (mmio + 0x100 + 0x08));

	regvalue = readl(mmio + 0x100 + (port_no * 0x80) + PORT_CMD);
	regvalue |= PORT_CMD_START;
	writel(regvalue, (mmio + 0x100 + (port_no * 0x80) + PORT_CMD));

	return 0;
}

static int mtk_ahci_init(struct device *dev, struct ahci_host_priv *hpriv)
{
	unsigned int port_no = 0;
	int count = 500;
	int ret = 0;

	/*init hba*/
	if (hba_init(dev, hpriv->mmio) < 0) {
		dev_err(dev, "[mtahci] init: hba init failed!\n");
		return -1;
	}

	/* Set Port 0 Hwinit registers */
	/*init port*/
	port_no = 0;
	if (port_init(dev, hpriv->mmio, port_no) < 0) {
		dev_err(dev, "[mtahci] init: port init failed!\n");
		return -1;
	}

	/* check sata mac status machine 500ms */
	while ((readl(hpriv->mmio + 0xb4) != 0x0100000A) && (count > 0)) {
		count--;
		udelay(1000);
	}

	if (count > 0) {
		ret = 1;
		dev_info(dev, "[ahci] receive good status OK! wait %dms\n", (500 - count));
	} else {
		dev_err(dev, "[ahci] receive good status timeout!\n");
		ret = -1;
	}

	dmam_free_coherent(dev, dma_sz, fis_mem, mem_dma);

	return 0;
}

static int ahci_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ahci_host_priv *hpriv;
	int rc;

	hpriv = ahci_platform_get_resources(pdev);
	if (IS_ERR(hpriv))
		return PTR_ERR(hpriv);

	rc = ahci_platform_enable_resources(hpriv);
	if (rc)
		return rc;

	of_property_read_u32(dev->of_node,
			     "ports-implemented", &hpriv->force_port_map);

	if (of_device_is_compatible(dev->of_node, "hisilicon,hisi-ahci"))
		hpriv->flags |= AHCI_HFLAG_NO_FBS | AHCI_HFLAG_NO_NCQ;

	/* MTK SATA init */
	rc = mtk_ahci_init(dev, hpriv);
	if (rc)
		goto disable_resources;

	rc = ahci_platform_init_host(pdev, hpriv, &ahci_port_info,
				     &ahci_platform_sht);
	if (rc)
		goto disable_resources;

	return 0;
disable_resources:
	ahci_platform_disable_resources(hpriv);
	return rc;
}

static SIMPLE_DEV_PM_OPS(ahci_pm_ops, ahci_platform_suspend,
			 ahci_platform_resume);

static const struct of_device_id ahci_of_match[] = {
	{ .compatible = "mediatek,mt7622-sata", },
	{},
};
MODULE_DEVICE_TABLE(of, ahci_of_match);

static struct platform_driver ahci_driver = {
	.probe = ahci_probe,
	.remove = ata_platform_remove_one,
	.driver = {
		.name = DRV_NAME,
		.of_match_table = ahci_of_match,
		.pm = &ahci_pm_ops,
	},
};
module_platform_driver(ahci_driver);

MODULE_DESCRIPTION("MediaTek MTK AHCI SATA Driver");
MODULE_AUTHOR("Long Cheng (Long) <long.cheng@mediatek.com>");
MODULE_LICENSE("GPL v2");


/* Copyright  2016 MediaTek Inc.
 * Author: Carlos Huang <carlos.huang@mediatek.com>
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
#include "raether.h"
#include "ra_switch.h"
#include "ra_mac.h"
#include "raeth_reg.h"

void reg_bit_zero(void __iomem *addr, unsigned int bit, unsigned int len)
{
	int reg_val;
	int i;

	reg_val = sys_reg_read(addr);
	for (i = 0; i < len; i++)
		reg_val &= ~(1 << (bit + i));
	sys_reg_write(addr, reg_val);
}

void reg_bit_one(void __iomem *addr, unsigned int bit, unsigned int len)
{
	unsigned int reg_val;
	unsigned int i;

	reg_val = sys_reg_read(addr);
	for (i = 0; i < len; i++)
		reg_val |= 1 << (bit + i);
	sys_reg_write(addr, reg_val);
}

u8 fe_cal_flag;
u8 fe_cal_flag_mdix;
u8 fe_cal_tx_offset_flag;
u8 fe_cal_tx_offset_flag_mdix;
u8 fe_cal_r50_flag;
u8 fe_cal_vbg_flag;
static u8 ephy_addr_base;
const u8 ZCAL_TO_R50OHM_TBL_100[64] = {
	127, 127, 127, 127, 127, 127, 126, 123, 120, 117, 114, 112, 110, 107,
	105, 103,
	101, 99, 97, 79, 77, 75, 74, 72, 70, 69, 67, 66, 65, 47, 46, 45,
	43, 42, 41, 40, 39, 38, 37, 36, 34, 34, 33, 32, 15, 14, 13, 12,
	11, 10, 10, 9, 8, 7, 7, 6, 5, 4, 4, 3, 2, 2, 1, 1
};

void tc_phy_write_g_reg(u8 port_num, u8 page_num,
			u8 reg_num, u32 reg_data)
{
	page_num = page_num << 12;
	mii_mgr_write(0, 31, page_num);	/* change Global page */
	mii_mgr_write(0, reg_num, reg_data);
}

void tc_phy_write_l_reg(u8 port_num, u8 page_num,
			u8 reg_num, u32 reg_data)
{
	page_num = 0x8000 | (page_num << 12);	/* Local register */
	mii_mgr_write(port_num, 31, page_num);	/* page_num */
	mii_mgr_write(port_num, reg_num, reg_data);
}

u32 tc_phy_read_g_reg(u8 port_num, u8 page_num, u8 reg_num)
{
	u32 phy_val;

	page_num = page_num << 12;
	mii_mgr_write(0, 31, page_num);	/* change Global page */
	mii_mgr_read(0, reg_num, &phy_val);
	return phy_val;
}

u32 tc_phy_read_l_reg(u8 port_num, u8 page_num, u8 reg_num)
{
	u32 phy_val;

	page_num = 0x8000 | (page_num << 12);	/* Local register */
	mii_mgr_write(port_num, 31, page_num);	/* page_num */
	mii_mgr_read(0, reg_num, &phy_val);
	return phy_val;
}

u8 all_fe_ana_cal_wait_txamp(u32 delay, u8 port_num)
{				/* for EN7512 FE // allen_20160616 */
	u8 all_ana_cal_status;
	u16 cnt, g7r24_temp;

	tc_phy_write_l_reg(FE_CAL_P0, 4, 23, (0x0000));
	/*l4r23 DA_CALIN_FLAG change g7r24[4]=0*/
	g7r24_temp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, g7r24_temp & (~0x10));
	/* udelay(10000); */
	/*l4r23_temp = tc_phy_read_l_reg(FE_CAL_P0, 4, 23);*/
	/*tc_phy_write_l_reg(FE_CAL_P0, 4, 23, (l4r23_temp | 0x0004));*/
	g7r24_temp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, g7r24_temp | 0x10);

	cnt = 1000;
	do {
		udelay(delay);
		cnt--;
		all_ana_cal_status =
		    ((tc_phy_read_g_reg(FE_CAL_P0, 7, 24) >> 1) & 0x1);
/*((tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 6) & 0x1);*/
	} while ((all_ana_cal_status == 0) && (cnt != 0));
	udelay((cnt * delay));
	udelay((cnt * delay));
	udelay((cnt * delay));

	tc_phy_write_l_reg(FE_CAL_P0, 4, 23, (0x0000));

	/*l4r23 clear DA_CALIN_FLAG change g7r24[4]=0*/
	g7r24_temp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, g7r24_temp & (~0x10));
	return all_ana_cal_status;
}

void fe_cal_tx_amp(u8 port_num, u32 delay)
{
	u8 all_ana_cal_status;
	u8 ad_cal_comp_out_init;
	u16 l3r25_temp, g7r24_tmp, l0r26_temp, l2r20_temp;
	u16 l2r23_temp = 0;
	int calibration_polarity;
	u8 tx_amp_reg_shift = 0;
	u8 tx_amp_temp = 0, cnt = 0, phyaddr, tx_amp_cnt = 0;

	phyaddr = port_num + ephy_addr_base;

	/* *** Tx Amp Cal start ********************** */
	/* tc_phy_write_l_reg(port_num, 2, 30, 0x0005); */
	/*g7r24[13]:0x1, RG_ANA_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x1000));

	/*g7r24[14]:0x1, RG_CAL_CKINV_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x4000));

	/*g7r24[12]:0x1, DA_TXVOS_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x1000));

	/*RG_TXG_CALEN =1 by port number*/
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x400));
	/*decide which port calibration RG_ZCALEN by port_num*/
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x1000));

	/*DA_PGA_MDIX_STASTUS_P0=0(L0R26[15:14] = 0x01) & RG_RX2TX_EN_P0=0(L2R20[10] =0),*/
	l0r26_temp = tc_phy_read_l_reg(FE_CAL_P0, 0, 26);
	l0r26_temp = l0r26_temp & (~0xc000);
	tc_phy_write_l_reg(FE_CAL_P0, 0, 26, (l0r26_temp | 0x4000));
	l2r20_temp = tc_phy_read_l_reg(FE_CAL_P0, 2, 20);
	l2r20_temp = l0r26_temp & (~0x400);
	tc_phy_write_l_reg(FE_CAL_P0, 0, 26, l2r20_temp);

	tc_phy_write_l_reg(port_num, 0, 0, 0x2100);
	tc_phy_write_l_reg(port_num, 0, 26, 0x5200);
	tc_phy_write_g_reg(port_num, 2, 25, 0x10c0);

	tc_phy_write_g_reg(port_num, 1, 26, (0x8000 | DAC_IN_2V));
	tc_phy_write_g_reg(port_num, 4, 21, (0x0800));	/* set default */
	tc_phy_write_l_reg(port_num, 0, 30, (0x02c0));
	tc_phy_write_l_reg(port_num, 4, 21, (0x0000));

	tc_phy_write_l_reg(FE_CAL_P0, 3, 25, (0xca00));
	tc_phy_write_l_reg(port_num, 3, 25, (0xca00));	/* 0xca00 */
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x0400));
	tc_phy_write_l_reg(port_num, 2, 23, (tx_amp_temp));

	all_ana_cal_status = all_fe_ana_cal_wait_txamp(delay, port_num);

	if (all_ana_cal_status == 0) {
		all_ana_cal_status = ANACAL_ERROR;
		pr_err(" FE Tx amp AnaCal ERROR! (init)  \r\n");
	}

	tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
	tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
	/*ad_cal_comp_out_init = (tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1;*/
	ad_cal_comp_out_init = tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0x1;
	if (ad_cal_comp_out_init == 1) {
		calibration_polarity = -1;
		tx_amp_temp = 0x10;
	} else {
		calibration_polarity = 1;
	}
	tx_amp_temp += calibration_polarity;
	cnt = 0;
	tx_amp_cnt = 0;
	while (all_ana_cal_status < ANACAL_ERROR) {
		tc_phy_write_l_reg(port_num, 2, 23, (tx_amp_temp));
		l2r23_temp = tc_phy_read_l_reg(port_num, 2, 23);

		cnt++;

		tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
		tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
		all_ana_cal_status = all_fe_ana_cal_wait_txamp(delay, port_num);

		if (((tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1) !=
		    ad_cal_comp_out_init) {
			all_ana_cal_status = ANACAL_FINISH;
			fe_cal_flag = 1;
		}
		if (all_ana_cal_status == 0) {
			all_ana_cal_status = ANACAL_ERROR;
			pr_err(" FE Tx amp AnaCal ERROR! (%d)  \r\n", cnt);
		} else if (((tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1) !=
			   ad_cal_comp_out_init) {
			tx_amp_cnt++;
			/* if(tx_amp_cnt >= 2) */
			/* { */
			all_ana_cal_status = ANACAL_FINISH;
			/* } */
			tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
			tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
			ad_cal_comp_out_init =
			    (tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1;
		} else {
			if ((cnt > 5) &&
			    ((l2r23_temp == 0x3f) || (l2r23_temp == 0x00))) {
				all_ana_cal_status = ANACAL_SATURATION;
				pr_err
				    (" Tx amp Cal Saturation(%d)(%x)(%x)\r\n",
				     cnt, tc_phy_read_l_reg(0, 3, 25),
				     tc_phy_read_l_reg(1, 3, 25));
				pr_err
				    (" Tx amp Cal Saturation(%x)(%x)(%x)\r\n",
				     tc_phy_read_l_reg(2, 3, 25),
				     tc_phy_read_l_reg(3, 3, 25),
				     tc_phy_read_l_reg(0, 2, 30));
				/* tx_amp_temp += calibration_polarity; */
			} else {
				tx_amp_temp += calibration_polarity;
			}
		}
	}

	if ((all_ana_cal_status == ANACAL_ERROR) ||
	    (all_ana_cal_status == ANACAL_SATURATION)) {
		l2r23_temp = tc_phy_read_l_reg(port_num, 2, 23);
		pr_err(" FE-%d Tx amp AnaCal Saturation! (%d)(0x%x)  \r\n",
		       phyaddr, cnt, l2r23_temp);
		tc_phy_write_l_reg(port_num, 2, 23,
				   ((tx_amp_temp << tx_amp_reg_shift)));
		l2r23_temp = tc_phy_read_l_reg(port_num, 2, 23);
		pr_err(" FE-%d Tx amp AnaCal Saturation! (%d)(0x%x)  \r\n",
		       phyaddr, cnt, l2r23_temp);
	} else {
		pr_info(" FE-%d Tx amp AnaCal Done! (%d)(0x%x)  \r\n",
			phyaddr, cnt, l2r23_temp);
#if defined(TCSUPPORT_CPU_EN7512)	/* allen_20160608 */
		if ((port_num == 2))	/*test for MSTC(7512 LAN2) */
			tc_phy_write_l_reg(port_num, 2, 23,
					   ((l2r23_temp + 2) <<
					     tx_amp_reg_shift));
		else if ((port_num == 3))
			tc_phy_write_l_reg(port_num, 2, 23,
					   ((l2r23_temp + 1) <<
					     tx_amp_reg_shift));
		else
#endif
		{
#if defined(TCSUPPORT_CPU_EN7521)
			if (port_num == 3)
				tc_phy_write_l_reg(port_num, 2, 23,
						   ((l2r23_temp - 1) <<
						    tx_amp_reg_shift));
			else
#endif
				tc_phy_write_l_reg(port_num, 2, 23,
						   ((l2r23_temp) <<
						    tx_amp_reg_shift));
		}
		fe_cal_flag = 1;
	}

	/*clear RG_CAL_CKINV/RG_ANA_CALEN/RG_TXVOS_CALEN*/
	/*g7r24[13]:0x0, RG_ANA_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x2000)));

	/*g7r24[14]:0x0, RG_CAL_CKINV_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x4000)));

	/*g7r24[12]:0x0, DA_TXVOS_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x1000)));

	tc_phy_write_l_reg(port_num, 3, 25, 0x0000);
	tc_phy_write_l_reg(FE_CAL_P0, 3, 25, 0x0000);
	/* *** Tx Amp Cal end *** */
	tc_phy_write_g_reg(port_num, 1, 26, 0x0000);

	tc_phy_write_l_reg(port_num, 0, 26, l0r26_temp);
	tc_phy_write_g_reg(port_num, 1, 26, 0x0000);

	tc_phy_write_l_reg(port_num, 2, 22, 0x4444);
	tc_phy_write_l_reg(port_num, 0, 0, 0x3100);
}

void fe_cal_tx_amp_mdix(u8 port_num, u32 delay)
{				/* for EN7512 */
	u8 all_ana_cal_status;
	u8 ad_cal_comp_out_init;
	u16 l3r25_temp, l4r26_temp, g7r24_tmp, l0r26_temp;
	u16 l2r20_temp;
	int calibration_polarity;
	u8 tx_amp_temp = 0, cnt = 0, phyaddr, tx_amp_cnt = 0;

	phyaddr = port_num + ephy_addr_base;

	/* *** Tx Amp Cal start ********************** */
	/* tc_phy_write_l_reg(port_num, 2, 30, 0x0005); */
	/*g7r24[13]:0x1, RG_ANA_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x1000));

	/*g7r24[14]:0x1, RG_CAL_CKINV_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x4000));

	/*g7r24[12]:0x1, DA_TXVOS_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x1000));

	/*RG_TXG_CALEN =1 by port number*/
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x400));
	/*decide which port calibration RG_ZCALEN by port_num*/
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x1000));

	/*DA_PGA_MDIX_STASTUS_P0=0(L0R26[15:14] = 0x01) & RG_RX2TX_EN_P0=0(L2R20[10] =0),*/
	l0r26_temp = tc_phy_read_l_reg(FE_CAL_P0, 0, 26);
	l0r26_temp = l0r26_temp & (~0xc000);
	tc_phy_write_l_reg(FE_CAL_P0, 0, 26, (l0r26_temp | 0x8000));
	l2r20_temp = tc_phy_read_l_reg(FE_CAL_P0, 2, 20);
	l2r20_temp = l0r26_temp | 0x400;
	tc_phy_write_l_reg(FE_CAL_P0, 0, 26, l2r20_temp);

	tc_phy_write_l_reg(port_num, 0, 0, 0x2100);
	tc_phy_write_l_reg(port_num, 0, 26, 0x5200);
	tc_phy_write_g_reg(port_num, 2, 25, 0x10c0);

	tc_phy_write_g_reg(port_num, 1, 26, (0x8000 | DAC_IN_2V));
	tc_phy_write_g_reg(port_num, 4, 21, (0x0800));	/* set default */
	tc_phy_write_l_reg(port_num, 0, 30, (0x02c0));
	tc_phy_write_l_reg(port_num, 4, 21, (0x0000));

	tc_phy_write_l_reg(FE_CAL_P0, 3, 25, (0xca00));
	tc_phy_write_l_reg(port_num, 3, 25, (0xca00));	/* 0xca00 */
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x0400));
/*DA_TX_I2MPB_MDIX L4R23[5:0]*/
	l4r26_temp = tc_phy_read_l_reg(port_num, 4, 26);
	tc_phy_write_l_reg(port_num, 4, 26, (l4r26_temp & ~(0x3f)));

	all_ana_cal_status = all_fe_ana_cal_wait_txamp(delay, port_num);

	if (all_ana_cal_status == 0) {
		all_ana_cal_status = ANACAL_ERROR;
		pr_err(" FE Tx amp mdix AnaCal ERROR! (init)  \r\n");
	}

	tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
	tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
	/*ad_cal_comp_out_init = (tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1;*/
	ad_cal_comp_out_init = tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0x1;
	if (ad_cal_comp_out_init == 1) {
		calibration_polarity = -1;
		tx_amp_temp = 0x10;
	} else {
		calibration_polarity = 1;
	}
	tx_amp_temp += calibration_polarity;
	cnt = 0;
	tx_amp_cnt = 0;
	while (all_ana_cal_status < ANACAL_ERROR) {
		l4r26_temp = tc_phy_read_l_reg(port_num, 4, 26);
		tc_phy_write_l_reg(port_num, 4, 26, (l4r26_temp | tx_amp_temp));
		l4r26_temp = (tc_phy_read_l_reg(port_num, 4, 26)) & 0x3f;

		cnt++;

		tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
		tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
		all_ana_cal_status = all_fe_ana_cal_wait_txamp(delay, port_num);

		if (((tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1) !=
		    ad_cal_comp_out_init) {
			all_ana_cal_status = ANACAL_FINISH;
			fe_cal_flag_mdix = 1;
		}
		if (all_ana_cal_status == 0) {
			all_ana_cal_status = ANACAL_ERROR;
			pr_err(" FE Tx amp mdix AnaCal ERROR! (%d)  \r\n", cnt);
		} else if (((tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1) !=
			   ad_cal_comp_out_init) {
			tx_amp_cnt++;
			/* if(tx_amp_cnt >= 2) */
			/* { */
			all_ana_cal_status = ANACAL_FINISH;
			/* } */
			tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
			tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
			ad_cal_comp_out_init =
			    (tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1;
		} else {
			if ((cnt > 5) &&
			    ((l4r26_temp == 0x3f) || (l4r26_temp == 0x00))) {
				all_ana_cal_status = ANACAL_SATURATION;
				pr_err
				    (" Tx amp Cal mdix Saturation(%d)(%x)(%x)\r\n",
				     cnt, tc_phy_read_l_reg(0, 3, 25),
				     tc_phy_read_l_reg(1, 3, 25));
				pr_err
				    (" Tx amp Cal mdix Saturation(%x)(%x)(%x)\r\n",
				     tc_phy_read_l_reg(2, 3, 25),
				     tc_phy_read_l_reg(3, 3, 25),
				     tc_phy_read_l_reg(0, 2, 30));
				/* tx_amp_temp += calibration_polarity; */
			} else {
				tx_amp_temp += calibration_polarity;
			}
		}
	}

	if ((all_ana_cal_status == ANACAL_ERROR) ||
	    (all_ana_cal_status == ANACAL_SATURATION)) {
		l4r26_temp = tc_phy_read_l_reg(port_num, 4, 26);
		pr_err(" FE-%d Tx amp AnaCal mdix Saturation! (%d)(0x%x)  \r\n",
		       phyaddr, cnt, l4r26_temp & 0x3f);
		tc_phy_write_l_reg(port_num, 4, 26,
				   (l4r26_temp | tx_amp_temp));
		l4r26_temp = tc_phy_read_l_reg(port_num, 4, 26);
		pr_err(" FE-%d Tx amp AnaCal mdix Saturation! (%d)(0x%x)  \r\n",
		       phyaddr, cnt, l4r26_temp & 0x3f);
	} else {
		pr_info(" FE-%d Tx amp mdix AnaCal Done! (%d)(0x%x)  \r\n",
			phyaddr, cnt, l4r26_temp & 0x3f);
#if defined(TCSUPPORT_CPU_EN7512)	/* allen_20160608 */
		if ((port_num == 2))	/*test for MSTC(7512 LAN2) */
			tc_phy_write_l_reg(port_num, 2, 23,
					   ((l2r23_temp + 2) <<
					     tx_amp_reg_shift));
		else if ((port_num == 3))
			tc_phy_write_l_reg(port_num, 2, 23,
					   ((l2r23_temp + 1) <<
					     tx_amp_reg_shift));
		else
#endif
		{
#if defined(TCSUPPORT_CPU_EN7521)
			if (port_num == 3)
				tc_phy_write_l_reg(port_num, 2, 23,
						   ((l2r23_temp - 1) <<
						    tx_amp_reg_shift));
			else
#endif
				tc_phy_write_l_reg(port_num, 4, 26,
						   l4r26_temp & 0x3f);
		}
		fe_cal_flag_mdix = 1;
	}

	/*clear RG_CAL_CKINV/RG_ANA_CALEN/RG_TXVOS_CALEN*/
	/*g7r24[13]:0x0, RG_ANA_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x2000)));

	/*g7r24[14]:0x0, RG_CAL_CKINV_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x4000)));

	/*g7r24[12]:0x0, DA_TXVOS_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x1000)));

	tc_phy_write_l_reg(port_num, 3, 25, 0x0000);
	tc_phy_write_l_reg(FE_CAL_P0, 3, 25, 0x0000);
	/* *** Tx Amp Cal end *** */
	tc_phy_write_g_reg(port_num, 1, 26, 0x0000);

	tc_phy_write_l_reg(port_num, 0, 26, l0r26_temp);
	tc_phy_write_g_reg(port_num, 1, 26, 0x0000);

	tc_phy_write_l_reg(port_num, 2, 22, 0x4444);
	tc_phy_write_l_reg(port_num, 0, 0, 0x3100);
}

u8 all_fe_ana_cal_wait(u32 delay, u8 port_num)
{		/* for EN7512 FE */
	u8 all_ana_cal_status;
	u16 cnt, g7r24_temp;

	tc_phy_write_l_reg(FE_CAL_P0, 4, 23, (0x0000));
	/*l4r23 DA_CALIN_FLAG change g7r24[4]=0*/
	g7r24_temp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, g7r24_temp & (~0x10));
	/* udelay(10000); */

	/*l4r23_temp = tc_phy_read_l_reg(FE_CAL_P0, 4, 23);*/
	/*tc_phy_write_l_reg(FE_CAL_P0, 4, 23, (l4r23_temp | 0x0004));*/
	g7r24_temp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, g7r24_temp | 0x10);
	cnt = 1000;
	do {
		udelay(delay);
		cnt--;
		all_ana_cal_status =
		    ((tc_phy_read_g_reg(FE_CAL_P0, 7, 24) >> 1) & 0x1);
/*((tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 6) & 0x1); change to g7r24[1]*/
	} while ((all_ana_cal_status == 0) && (cnt != 0));
	tc_phy_write_l_reg(FE_CAL_P0, 4, 23, (0x0000));
	/*l4r23 clear DA_CALIN_FLAG change g7r24[4]=0*/
	g7r24_temp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, g7r24_temp & (~0x10));
	return all_ana_cal_status;
}

void fe_cal_tx_offset(u8 port_num, u32 delay)
{				/* for EN7512 */
	u8 all_ana_cal_status;
	u8 ad_cal_comp_out_init;
	u16 l3r25_temp, l2r20_temp, g7r24_tmp;
	u16 g4r21_temp, l0r30_temp, l4r17_temp, l0r26_temp;
	int calibration_polarity, tx_offset_temp;
	u16 cal_temp = 0;
	u8 tx_offset_reg_shift;
	u8 cnt = 0, phyaddr, tx_amp_cnt = 0;

	phyaddr = port_num + ephy_addr_base;

	tc_phy_write_l_reg(port_num, 0, 0, 0x2100);
	tc_phy_write_l_reg(port_num, 0, 26, 0x5200);	/* fix mdi */
/*l3r25[15]:rg_cal_ckinv, [14]:rg_ana_calen, [13]:rg_rext_calen, [11]:rg_txvos_calen*/
	/*l3r25_temp = tc_phy_read_l_reg(FE_CAL_P0, 3, 25);*/
	/*tc_phy_write_l_reg(FE_CAL_P0, 3, 25, (0x0000));*/
	/*tc_phy_write_l_reg(FE_CAL_P0, 3, 25, (0x4800));*/
	/*tc_phy_write_l_reg(port_num, 3, 25, (0x4c00));*/

	/*g7r24[13]:0x1, RG_ANA_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x1000));

	/*g7r24[14]:0x0, RG_CAL_CKINV_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x4000)));

	/*g7r24[12]:0x1, DA_TXVOS_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x1000));

	/*RG_TXG_CALEN =1 by port number*/
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x400));
	/*decide which port calibration RG_ZCALEN by port_num*/
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x1000));
	/*DA_PGA_MDIX_STASTUS_P0=0(L0R26[15:14] = 0x01) & RG_RX2TX_EN_P0=0(L2R20[10] =0),*/
	l0r26_temp = tc_phy_read_l_reg(FE_CAL_P0, 0, 26);
	l0r26_temp = l0r26_temp & (~0xc000);
	tc_phy_write_l_reg(FE_CAL_P0, 0, 26, (l0r26_temp | 0x4000));
	l2r20_temp = tc_phy_read_l_reg(FE_CAL_P0, 2, 20);
	l2r20_temp = l0r26_temp & (~0x400);
	tc_phy_write_l_reg(FE_CAL_P0, 0, 26, l2r20_temp);

	/*// g4r21[11]:Hw bypass tx offset cal, Fw cal*/
	g4r21_temp = tc_phy_read_g_reg(port_num, 4, 21);
	tc_phy_write_g_reg(port_num, 4, 21, (g4r21_temp | 0x0800));

	/*l0r30[9], [7], [6], [1]*/
	l0r30_temp = tc_phy_read_l_reg(port_num, 0, 30);
	tc_phy_write_l_reg(port_num, 0, 30, (l0r30_temp | 0x02c0));

	tx_offset_temp = TX_AMP_OFFSET_0MV;
	tx_offset_reg_shift = 8;
	tc_phy_write_g_reg(port_num, 1, 26, (0x8000 | DAC_IN_0V));

	tc_phy_write_l_reg(port_num, 4, 17, (0x0000));
	l4r17_temp = tc_phy_read_l_reg(port_num, 4, 17);
	tc_phy_write_l_reg(port_num, 4, 17,
			   l4r17_temp |
			   (tx_offset_temp << tx_offset_reg_shift));
/*wat AD_CAL_CLK = 1*/
	all_ana_cal_status = all_fe_ana_cal_wait(40, port_num);
	if (all_ana_cal_status == 0) {
		all_ana_cal_status = ANACAL_ERROR;
		pr_err(" FE Tx offset AnaCal ERROR! (init)  \r\n");
	}

	tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
	tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
/*GET AD_CAL_COMP_OUT g724[0]*/
	/*ad_cal_comp_out_init = (tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1;*/
	ad_cal_comp_out_init = tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0x1;

	if (ad_cal_comp_out_init == 1)
		calibration_polarity = -1;
	else
		calibration_polarity = 1;

	cnt = 0;
	if (port_num == 0)
		tx_amp_cnt = 1;
	else
		tx_amp_cnt = 0;
	while ((all_ana_cal_status < ANACAL_ERROR) && (cnt < 254)) {
		cnt++;

		/* tx_offset_temp += calibration_polarity; */
		if (tx_offset_temp >= 0) {
			cal_temp = tx_offset_temp;
		} else {
			cal_temp =
			    (1 << (TX_AMP_OFFSET_VALID_BITS - 1)) |
			    abs(tx_offset_temp);
		}
		/* l4r17_temp = tc_phy_read_l_reg(port_num, 4, 17); */
		tc_phy_write_l_reg(port_num, 4, 17,
				   (cal_temp << tx_offset_reg_shift));

		tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
		tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
		all_ana_cal_status = all_fe_ana_cal_wait(40, port_num);

		if (all_ana_cal_status == 0) {
			all_ana_cal_status = ANACAL_ERROR;
			pr_err(" FE Tx offset AnaCal ERROR! (%d)  \r\n", cnt);
		/*} else if (((tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1) !=*/
		} else if ((tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0x1) !=
			   ad_cal_comp_out_init) {
			tx_amp_cnt++;
			if (tx_amp_cnt >= 2)
				all_ana_cal_status = ANACAL_FINISH;
			tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
			tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
			ad_cal_comp_out_init =
			    (tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1;
		} else {
			l4r17_temp = tc_phy_read_l_reg(port_num, 4, 17);
			if ((cnt > 3) &&
			    ((tx_offset_temp == (-31)) ||
			     (tx_offset_temp == 31))) {
				if (calibration_polarity == 1)
					calibration_polarity = -1;
				else
					calibration_polarity = 1;
				tx_offset_temp += calibration_polarity;
			} else {
				tx_offset_temp += calibration_polarity;
			}
		}
	}

	if ((all_ana_cal_status == ANACAL_ERROR) ||
	    (all_ana_cal_status == ANACAL_SATURATION)) {
		tx_offset_temp = TX_AMP_OFFSET_0MV;
		l4r17_temp = tc_phy_read_l_reg(port_num, 4, 17);
		tc_phy_write_l_reg(port_num, 4, 17,
				   (l4r17_temp |
				    (tx_offset_temp << tx_offset_reg_shift)));
	} else {
		pr_info(" FE-%d Tx offset AnaCal Done! (%d)(%d)(0x%x)  \r\n",
			phyaddr, cnt, tx_offset_temp, cal_temp);
		fe_cal_tx_offset_flag = 1;
	}
	/*clear RG_CAL_CKINV/RG_ANA_CALEN/RG_TXVOS_CALEN*/
	/*g7r24[13]:0x0, RG_ANA_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x2000)));

	/*g7r24[14]:0x0, RG_CAL_CKINV_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x4000)));

	/*g7r24[12]:0x0, DA_TXVOS_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x1000)));

	/* tc_phy_write_g_reg(port_num, 1, 26, 0x0000); */
	/* l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25); */
	tc_phy_write_l_reg(port_num, 3, 25, 0x0000);
	tc_phy_write_l_reg(FE_CAL_P0, 3, 25, 0x0000);
	/* tc_phy_write_l_reg(port_num, 0, 30, 0x0000); */
}

void fe_cal_tx_offset_mdix(u8 port_num, u32 delay)
{				/* for MT7622 */
	u8 all_ana_cal_status;
	u8 ad_cal_comp_out_init;
	u16 l3r25_temp, l2r20_temp, g7r24_tmp, l4r26_temp;
	u16 g4r21_temp, l0r30_temp, l4r17_temp, l0r26_temp;
	int calibration_polarity, tx_offset_temp;
	u16 cal_temp = 0;
	u8 tx_offset_reg_shift;
	u8 cnt = 0, phyaddr, tx_amp_cnt = 0;

	phyaddr = port_num + ephy_addr_base;

	tc_phy_write_l_reg(port_num, 0, 0, 0x2100);
	tc_phy_write_l_reg(port_num, 0, 26, 0x5200);	/* fix mdi */
/*l3r25[15]:rg_cal_ckinv, [14]:rg_ana_calen, [13]:rg_rext_calen, [11]:rg_txvos_calen*/
	/*l3r25_temp = tc_phy_read_l_reg(FE_CAL_P0, 3, 25);*/
	/*tc_phy_write_l_reg(FE_CAL_P0, 3, 25, (0x0000));*/
	/*tc_phy_write_l_reg(FE_CAL_P0, 3, 25, (0x4800));*/
	/*tc_phy_write_l_reg(port_num, 3, 25, (0x4c00));*/

	/*g7r24[13]:0x1, RG_ANA_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x1000));

	/*g7r24[14]:0x0, RG_CAL_CKINV_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x4000)));

	/*g7r24[12]:0x1, DA_TXVOS_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x1000));

	/*RG_TXG_CALEN =1 by port number*/
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x400));

	/*decide which port calibration RG_ZCALEN by port_num*/
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x1000));

	/*DA_PGA_MDIX_STASTUS_P0=0(L0R26[15:14] = 0x01) & RG_RX2TX_EN_P0=0(L2R20[10] =0),*/
	l0r26_temp = tc_phy_read_l_reg(FE_CAL_P0, 0, 26);
	l0r26_temp = l0r26_temp & (~0xc000);
	tc_phy_write_l_reg(FE_CAL_P0, 0, 26, (l0r26_temp | 0x8000));
	l2r20_temp = tc_phy_read_l_reg(FE_CAL_P0, 2, 20);
	l2r20_temp = l0r26_temp | 0x400;
	tc_phy_write_l_reg(FE_CAL_P0, 0, 26, l2r20_temp);

	/*// g4r21[11]:Hw bypass tx offset cal, Fw cal*/
	g4r21_temp = tc_phy_read_g_reg(port_num, 4, 21);
	tc_phy_write_g_reg(port_num, 4, 21, (g4r21_temp | 0x0800));

	/*l0r30[9], [7], [6], [1]*/
	l0r30_temp = tc_phy_read_l_reg(port_num, 0, 30);
	tc_phy_write_l_reg(port_num, 0, 30, (l0r30_temp | 0x02c0));

	tx_offset_temp = TX_AMP_OFFSET_0MV;
	tx_offset_reg_shift = 8;
	tc_phy_write_g_reg(port_num, 1, 26, (0x8000 | DAC_IN_0V));

	tc_phy_write_l_reg(port_num, 4, 26, (0x0000));
	l4r26_temp = tc_phy_read_l_reg(port_num, 4, 26);
	tc_phy_write_l_reg(port_num, 4, 26,
			   l4r26_temp |
			   (tx_offset_temp << tx_offset_reg_shift));
/*wat AD_CAL_CLK = 1*/
	all_ana_cal_status = all_fe_ana_cal_wait(40, port_num);
	if (all_ana_cal_status == 0) {
		all_ana_cal_status = ANACAL_ERROR;
		pr_err(" FE Tx offset mdix AnaCal ERROR! (init)  \r\n");
	}

	tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
	tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
/*GET AD_CAL_COMP_OUT g724[0]*/
	/*ad_cal_comp_out_init = (tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1;*/
	ad_cal_comp_out_init = tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0x1;

	if (ad_cal_comp_out_init == 1)
		calibration_polarity = -1;
	else
		calibration_polarity = 1;

	cnt = 0;
	if (port_num == 0)
		tx_amp_cnt = 1;
	else
		tx_amp_cnt = 0;
	while ((all_ana_cal_status < ANACAL_ERROR) && (cnt < 254)) {
		cnt++;

		/* tx_offset_temp += calibration_polarity; */
		if (tx_offset_temp >= 0) {
			cal_temp = tx_offset_temp;
		} else {
			cal_temp =
			    (1 << (TX_AMP_OFFSET_VALID_BITS - 1)) |
			    abs(tx_offset_temp);
		}
		/* l4r17_temp = tc_phy_read_l_reg(port_num, 4, 17); */
		tc_phy_write_l_reg(port_num, 4, 26,
				   (cal_temp << tx_offset_reg_shift));

		tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
		tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
		all_ana_cal_status = all_fe_ana_cal_wait(40, port_num);

		if (all_ana_cal_status == 0) {
			all_ana_cal_status = ANACAL_ERROR;
			pr_err(" FE Tx offset mdix AnaCal ERROR! (%d)  \r\n", cnt);
		/*} else if (((tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1) !=*/
		} else if ((tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0x1) !=
			   ad_cal_comp_out_init) {
			tx_amp_cnt++;
			if (tx_amp_cnt >= 2)
				all_ana_cal_status = ANACAL_FINISH;
			tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
			tc_phy_read_l_reg(FE_CAL_P0, 4, 23);
			ad_cal_comp_out_init =
			    (tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1;
		} else {
			l4r26_temp = tc_phy_read_l_reg(port_num, 4, 26);
			if ((cnt > 3) &&
			    ((tx_offset_temp == (-31)) ||
			     (tx_offset_temp == 31))) {
				if (calibration_polarity == 1)
					calibration_polarity = -1;
				else
					calibration_polarity = 1;
				tx_offset_temp += calibration_polarity;
			} else {
				tx_offset_temp += calibration_polarity;
			}
		}
	}

	if ((all_ana_cal_status == ANACAL_ERROR) ||
	    (all_ana_cal_status == ANACAL_SATURATION)) {
		tx_offset_temp = TX_AMP_OFFSET_0MV;
		l4r17_temp = tc_phy_read_l_reg(port_num, 4, 26);
		tc_phy_write_l_reg(port_num, 4, 26,
				   (l4r17_temp |
				    (tx_offset_temp << tx_offset_reg_shift)));
	} else {
		pr_info(" FE-%d Tx offsetmdix  AnaCal Done! (%d)(%d)(0x%x)  \r\n",
			phyaddr, cnt, tx_offset_temp, cal_temp);
		fe_cal_tx_offset_flag_mdix = 1;
	}
	/*clear RG_CAL_CKINV/RG_ANA_CALEN/RG_TXVOS_CALEN*/
	/*g7r24[13]:0x0, RG_ANA_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x2000)));

	/*g7r24[14]:0x0, RG_CAL_CKINV_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x4000)));

	/*g7r24[12]:0x0, DA_TXVOS_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x1000)));

	/* tc_phy_write_g_reg(port_num, 1, 26, 0x0000); */
	/* l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25); */
	tc_phy_write_l_reg(port_num, 3, 25, 0x0000);
	tc_phy_write_l_reg(FE_CAL_P0, 3, 25, 0x0000);
	/* tc_phy_write_l_reg(port_num, 0, 30, 0x0000); */
}

void fe_cal_r50(u8 port_num, u32 delay)
{
	u8 rg_zcal_ctrl, all_ana_cal_status, rg_zcal_ctrl_tx, rg_zcal_ctrl_rx;
	u8 ad_cal_comp_out_init;
	u16 l3r25_temp, l4r22_temp, l0r4, g7r24_tmp;
	int calibration_polarity;

	u8 cnt = 0, phyaddr;

	phyaddr = port_num + ephy_addr_base;

	/*g2r25[7:5]:0x110, BG voltage output*/
	tc_phy_write_g_reg(port_num, 2, 25, 0x10c0);

	/*change to g7r24*/
	/*l3r25_temp = tc_phy_read_l_reg(FE_CAL_P0, 3, 25);*/
	/*tc_phy_write_l_reg(FE_CAL_P0, 3, 25, (0xc000));*/
	/*g7r24[13]:0x01, RG_ANA_CALEN_P0=1*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x2000));
	/*g7r24[14]:0x01, RG_CAL_CKINV_P0=1*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x4000));

	/*g7r24[12]:0x01, DA_TXVOS_CALEN_P0=1*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x1000));

	/*RG_REXT_CALEN l2r25[13] = 0*/
	l3r25_temp = tc_phy_read_l_reg(FE_CAL_P0, 3, 25);
	tc_phy_write_l_reg(FE_CAL_P0, 3, 25, (l3r25_temp & (~0x2000)));

	/*decide which port calibration RG_ZCALEN by port_num*/
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x1000));

	rg_zcal_ctrl = 0x20;	/* start with 0 dB */
	/*l3r26_temp = (tc_phy_read_l_reg(FE_CAL_P0, 3, 26) & (~0x003f));*/
	/*tc_phy_write_l_reg(FE_CAL_P0, 3, 26, (rg_zcal_ctrl));*/
	g7r24_tmp = (tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & (~0xfc0));
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, g7r24_tmp);

	/*wait AD_CAL_COMP_OUT = 1*/
	all_ana_cal_status = all_fe_ana_cal_wait(delay, port_num);
	if (all_ana_cal_status == 0) {
		all_ana_cal_status = ANACAL_ERROR;
		pr_err(" FE R50 AnaCal ERROR! (init)   \r\n");
	}
	/*AD_CAL_COMP_OUT change t0 G7R24[0]*/
	/*ad_cal_comp_out_init = (tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1;*/
	ad_cal_comp_out_init = tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0x1;
	if (ad_cal_comp_out_init == 1)
		calibration_polarity = -1;
	else
		calibration_polarity = 1;

	cnt = 0;
	while ((all_ana_cal_status < ANACAL_ERROR) && (cnt < 254)) {
		cnt++;

		rg_zcal_ctrl += calibration_polarity;
		/*RG_ZCAL_CTRL change to G7R24[11:6]*/
		/*tc_phy_write_l_reg(FE_CAL_P0, 3, 26, (rg_zcal_ctrl));*/
		g7r24_tmp = (tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & (~0xfc0));
		tc_phy_write_g_reg(FE_CAL_P0, 7, 24, g7r24_tmp | ((rg_zcal_ctrl & 0x3f) << 6));

		all_ana_cal_status = all_fe_ana_cal_wait(delay, port_num);

		if (all_ana_cal_status == 0) {
			all_ana_cal_status = ANACAL_ERROR;
			pr_err(" FE R50 AnaCal ERROR! (%d)  \r\n", cnt);
		} else if ((tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0x1) !=
			ad_cal_comp_out_init) {
			all_ana_cal_status = ANACAL_FINISH;
		} else {
			if ((rg_zcal_ctrl == 0x3F) || (rg_zcal_ctrl == 0x00)) {
				all_ana_cal_status = ANACAL_SATURATION;
				pr_err(" FE R50 AnaCal Saturation! (%d)  \r\n",
				       cnt);
			} else {
				l0r4 = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
				l0r4 = l0r4 & 0x1;
				pr_err(" FE-%d R50(%d)(%d)(0x%x)(g7r24[11:6]=0x%x)\r\n",
				       phyaddr, cnt, l0r4, rg_zcal_ctrl,
				       (tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0xfc));
			}
		}
	}

	/* pr_err(" FE R50 AnaCal zcal_ctrl (%d)  \r\n", (rg_zcal_ctrl)); */

	if (phyaddr == 11) {
		rg_zcal_ctrl_tx = ZCAL_TO_R50OHM_TBL_100[(rg_zcal_ctrl - 2)];
		rg_zcal_ctrl_rx = ZCAL_TO_R50OHM_TBL_100[(rg_zcal_ctrl - 8)];
	} else {
		rg_zcal_ctrl_tx = ZCAL_TO_R50OHM_TBL_100[(rg_zcal_ctrl + 2)];
		rg_zcal_ctrl_rx = ZCAL_TO_R50OHM_TBL_100[(rg_zcal_ctrl - 4)];
	}
	/* pr_err(" FE R50 AnaCal zcal_ctrl (0x%x)  \r\n", rg_zcal_ctrl); */

	if ((all_ana_cal_status == ANACAL_ERROR) ||
	    (all_ana_cal_status == ANACAL_SATURATION)) {
		rg_zcal_ctrl = 0x20;	/* 0 dB */
	} else {
		pr_info(" FE-%d R50 AnaCal Done! (%d)(0x%x)	\r\n",
			phyaddr, cnt, rg_zcal_ctrl);
		fe_cal_r50_flag = 1;
	}
	tc_phy_write_l_reg(port_num, 4, 22, ((rg_zcal_ctrl_tx << 8)));
	l4r22_temp = tc_phy_read_l_reg(port_num, 4, 22);
	tc_phy_write_l_reg(port_num, 4, 22,
			   (l4r22_temp | (rg_zcal_ctrl_rx << 0)));
	/*clear RG_CAL_CKINV/RG_ANA_CALEN/RG_TXVOS_CALEN*/
	/*g7r24[13]:0x0, RG_ANA_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x2000)));

	/*g7r24[14]:0x0, RG_CAL_CKINV_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x4000)));

	/*g7r24[12]:0x0, DA_TXVOS_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x1000)));

	tc_phy_write_l_reg(port_num, 3, 25, 0x0000);
	tc_phy_write_l_reg(FE_CAL_P0, 3, 25, 0x0000);
}

void fe_cal_vbg(u8 port_num, u32 delay)
{
	u8 rg_zcal_ctrl, all_ana_cal_status;
	u8 ad_cal_comp_out_init;
	u16 l3r25_temp, l0r4, g7r24_tmp, l3r26_temp;
	int calibration_polarity;
	u16 g2r22_temp;
	u8 cnt = 0, phyaddr;

	phyaddr = port_num + ephy_addr_base;
	/*g2r25[7:5]:0x110, BG voltage output*/
	tc_phy_write_g_reg(port_num, 2, 25, 0x10c0);
	/*change to g7r24*/
	/*l3r25_temp = tc_phy_read_l_reg(FE_CAL_P0, 3, 25);*/
	/*tc_phy_write_l_reg(FE_CAL_P0, 3, 25, (0xc000));*/
	/*g7r24[13]:0x01, RG_ANA_CALEN_P0=1*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x2000));
	/*g7r24[14]:0x01, RG_CAL_CKINV_P0=1*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x4000));

	/*g7r24[12]:0x01, DA_TXVOS_CALEN_P0=1*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp | 0x1000));

	/*RG_REXT_CALEN l2r25[13] = 1*/
	l3r25_temp = tc_phy_read_l_reg(FE_CAL_P0, 3, 25);
	tc_phy_write_l_reg(FE_CAL_P0, 3, 25, (l3r25_temp | 0x2000));

	/*decide which port calibration RG_ZCALEN by port_num*/
	l3r25_temp = tc_phy_read_l_reg(port_num, 3, 25);
	tc_phy_write_l_reg(port_num, 3, 25, (l3r25_temp | 0x1000));

	rg_zcal_ctrl = 0x20;	/* start with 0 dB */
	/*l3r26_temp = (tc_phy_read_l_reg(FE_CAL_P0, 3, 26) & (~0x003f));*/
	/*tc_phy_write_l_reg(FE_CAL_P0, 3, 26, (rg_zcal_ctrl));*/
	g7r24_tmp = (tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & (~0xfc0));
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, g7r24_tmp);

	/*wait AD_CAL_COMP_OUT = 1*/
	all_ana_cal_status = all_fe_ana_cal_wait(delay, port_num);
	if (all_ana_cal_status == 0) {
		all_ana_cal_status = ANACAL_ERROR;
		pr_err(" FE R50 AnaCal ERROR! (init)   \r\n");
	}
	/*AD_CAL_COMP_OUT change t0 G7R24[0]*/
	/*ad_cal_comp_out_init = (tc_phy_read_l_reg(FE_CAL_P0, 4, 23) >> 4) & 0x1;*/
	ad_cal_comp_out_init = tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0x1;
	if (ad_cal_comp_out_init == 1)
		calibration_polarity = -1;
	else
		calibration_polarity = 1;

	cnt = 0;
	while ((all_ana_cal_status < ANACAL_ERROR) && (cnt < 254)) {
		cnt++;

		rg_zcal_ctrl += calibration_polarity;
		/*RG_ZCAL_CTRL change to G7R24[11:6]*/
		/*tc_phy_write_l_reg(FE_CAL_P0, 3, 26, (rg_zcal_ctrl));*/
		g7r24_tmp = (tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & (~0xfc0));
		tc_phy_write_g_reg(FE_CAL_P0, 7, 24, g7r24_tmp | ((rg_zcal_ctrl & 0x3f) << 6));

		all_ana_cal_status = all_fe_ana_cal_wait(delay, port_num);

		if (all_ana_cal_status == 0) {
			all_ana_cal_status = ANACAL_ERROR;
			pr_err(" FE R50 AnaCal ERROR! (%d)  \r\n", cnt);
		} else if ((tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0x1) !=
			ad_cal_comp_out_init) {
			all_ana_cal_status = ANACAL_FINISH;
		} else {
			if ((rg_zcal_ctrl == 0x3F) || (rg_zcal_ctrl == 0x00)) {
				all_ana_cal_status = ANACAL_SATURATION;
				pr_err(" FE R50 AnaCal Saturation! (%d)  \r\n",
				       cnt);
			} else {
				l0r4 = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
				l0r4 = l0r4 & 0x1;
				pr_err(" FE-%d R50(%d)(%d)(0x%x)(g7r24[11:6]=0x%x)\r\n",
				       phyaddr, cnt, l0r4, rg_zcal_ctrl,
				       (tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & 0xfc));
			}
		}
	}
	if ((all_ana_cal_status == ANACAL_ERROR) ||
	    (all_ana_cal_status == ANACAL_SATURATION)) {
		rg_zcal_ctrl = 0x20;	/* 0 dB */
	} else {
		pr_info(" FE-%d VBG Cal Done! (%d)(0x%x)	\r\n",
			phyaddr, cnt, rg_zcal_ctrl);
		fe_cal_vbg_flag = 1;
	}
	/* pr_err(" FE R50 AnaCal zcal_ctrl (%d)  \r\n", (rg_zcal_ctrl)); */
/*RG_REXT_TRIM_P0[5:0]= RG_ZCAL_CTRL_P0[5:0]*/
	rg_zcal_ctrl = (tc_phy_read_g_reg(FE_CAL_P0, 7, 24) & (~0xfc0)) >> 6;
	l3r26_temp = tc_phy_read_l_reg(FE_CAL_P0, 3, 26);
	tc_phy_write_l_reg(FE_CAL_P0, 3, 26, l3r26_temp | ((rg_zcal_ctrl & 0x3f) << 6));
/*RG_BG_RASEL[2:0](g2r22[11:9])= RG_ZCAL_CTRL_P0[5:3](g7r24[11:9]).*/
	rg_zcal_ctrl = rg_zcal_ctrl & 0x38;
	g2r22_temp = tc_phy_read_g_reg(FE_CAL_P0, 2, 22);
	tc_phy_write_l_reg(FE_CAL_P0, 3, 26, g2r22_temp | rg_zcal_ctrl);

	/*clear RG_CAL_CKINV/RG_ANA_CALEN/RG_TXVOS_CALEN*/
	/*g7r24[13]:0x0, RG_ANA_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x2000)));

	/*g7r24[14]:0x0, RG_CAL_CKINV_P0*/
	g7r24_tmp = tc_phy_read_g_reg(FE_CAL_P0, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x4000)));

	/*g7r24[12]:0x0, DA_TXVOS_CALEN_P0*/
	g7r24_tmp = tc_phy_read_g_reg(port_num, 7, 24);
	tc_phy_write_g_reg(FE_CAL_P0, 7, 24, (g7r24_tmp & (~0x1000)));

	tc_phy_write_l_reg(port_num, 3, 25, 0x0000);
	tc_phy_write_l_reg(FE_CAL_P0, 3, 25, 0x0000);
}

/* harry */
static void do_fe_phy_all_analog_cal(u8 port_num)
{
	u16 l0r26_temp;
	u8 cnt = 0, phyaddr;

	phyaddr = port_num + ephy_addr_base;

	l0r26_temp = tc_phy_read_l_reg(port_num, 0, 26);
	tc_phy_write_l_reg(port_num, 0, 26, 0x5600);
	tc_phy_write_l_reg(port_num, 4, 21, 0x0000);
	tc_phy_write_l_reg(port_num, 0, 0, 0x2100);

	/*****VBG & IEXT Calibration*****/
	cnt = 0;
	while ((fe_cal_vbg_flag == 0) && (cnt < 0x3)) {
		fe_cal_vbg(port_num, 1);	/* allen_20160608 */
		cnt++;
		if (fe_cal_vbg_flag == 0)
			pr_info(" FE-%d VBG wait! (%d)  \r\n", phyaddr, cnt);
	}
	/**** VBG & IEXT Calibration end ****/

	/* *** R50 Cal start *************************************** */
	cnt = 0;
	while ((fe_cal_r50_flag == 0) && (cnt < 0x3)) {
		fe_cal_r50(port_num, 1);	/* allen_20160608 */
		cnt++;
		if (fe_cal_r50_flag == 0)
			pr_info(" FE-%d R50 wait! (%d)  \r\n", phyaddr, cnt);
	}
	/* *** R50 Cal end *** */

	/* *** Tx offset Cal start ********************************* */

#if 1
	cnt = 0;
	while ((fe_cal_tx_offset_flag == 0) && (cnt < 0x3)) {
		fe_cal_tx_offset(port_num, 100);
		cnt++;
		if (fe_cal_tx_offset_flag == 0)
			pr_info(" FE-%d Tx offset AnaCal wait! (%d)  \r\n",
				phyaddr, cnt);
	}
#endif

#if 1
	cnt = 0;
	while ((fe_cal_tx_offset_flag_mdix == 0) && (cnt < 0x3)) {
		fe_cal_tx_offset_mdix(port_num, 100);
		cnt++;
		if (fe_cal_tx_offset_flag_mdix == 0)
			pr_info(" FE-%d Tx offset AnaCal Mdix wait! (%d)  \r\n",
				phyaddr, cnt);
	}
#endif
	/* *** Tx offset Cal end *** */

	/* *** Tx Amp Cal start ************************************** */
	cnt = 0;
	while ((fe_cal_flag == 0) && (cnt < 0x3)) {
		fe_cal_tx_amp(port_num, 300);	/* allen_20160608 */

		cnt++;
		if (fe_cal_flag == 0)
			pr_info(" FE-%d Tx amp AnaCal wait! (%d)  \r\n",
				phyaddr, cnt);
	}

	cnt = 0;
	while ((fe_cal_flag_mdix == 0) && (cnt < 0x3)) {
		fe_cal_tx_amp_mdix(port_num, 300);	/* allen_20160608 */

		cnt++;
		if (fe_cal_flag_mdix == 0)
			pr_info(" FE-%d Tx amp AnaCal mdix wait! (%d)  \r\n",
				phyaddr, cnt);
	}
}

static void mt7622_ephy_cal(void)
{
	int i;

	for (i = 0; i < 5; i++)
		do_fe_phy_all_analog_cal(i);
}

static void wait_loop(void)
{
	int i;
	int read_data;

	for (i = 0; i < 320; i = i + 1)
		read_data = sys_reg_read(RALINK_ETH_SW_BASE + 0x108);
}

static void trgmii_calibration_7623(void)
{
	/* minimum delay for all correct */
	unsigned int tap_a[5] = {
		0, 0, 0, 0, 0
	};
	/* maximum delay for all correct */
	unsigned int tap_b[5] = {
		0, 0, 0, 0, 0
	};
	unsigned int final_tap[5];
	unsigned int rxc_step_size;
	unsigned int rxd_step_size;
	unsigned int read_data;
	unsigned int tmp;
	unsigned int rd_wd;
	int i;
	unsigned int err_cnt[5];
	unsigned int init_toggle_data;
	unsigned int err_flag[5];
	unsigned int err_total_flag;
	unsigned int training_word;
	unsigned int rd_tap;

	void __iomem *TRGMII_7623_base;
	void __iomem *TRGMII_7623_RD_0;
	void __iomem *temp_addr;

	TRGMII_7623_base = ETHDMASYS_ETH_SW_BASE + 0x0300;
	TRGMII_7623_RD_0 = TRGMII_7623_base + 0x10;
	rxd_step_size = 0x1;
	rxc_step_size = 0x4;
	init_toggle_data = 0x00000055;
	training_word = 0x000000AC;

	/* RX clock gating in MT7623 */
	reg_bit_zero(TRGMII_7623_base + 0x04, 30, 2);
	/* Assert RX  reset in MT7623 */
	reg_bit_one(TRGMII_7623_base + 0x00, 31, 1);
	/* Set TX OE edge in  MT7623 */
	reg_bit_one(TRGMII_7623_base + 0x78, 13, 1);
	/* Disable RX clock gating in MT7623 */
	reg_bit_one(TRGMII_7623_base + 0x04, 30, 2);
	/* Release RX reset in MT7623 */
	reg_bit_zero(TRGMII_7623_base, 31, 1);

	for (i = 0; i < 5; i++)
		/* Set bslip_en = 1 */
		reg_bit_one(TRGMII_7623_RD_0 + i * 8, 31, 1);

	/* Enable Training Mode in MT7530 */
	mii_mgr_read(0x1F, 0x7A40, &read_data);
	read_data |= 0xc0000000;
	mii_mgr_write(0x1F, 0x7A40, read_data);

	err_total_flag = 0;
	read_data = 0x0;
	while (err_total_flag == 0 && read_data != 0x68) {
		/* Enable EDGE CHK in MT7623 */
		for (i = 0; i < 5; i++) {
			reg_bit_zero(TRGMII_7623_RD_0 + i * 8, 28, 4);
			reg_bit_one(TRGMII_7623_RD_0 + i * 8, 31, 1);
		}
		wait_loop();
		err_total_flag = 1;
		for (i = 0; i < 5; i++) {
			tmp = sys_reg_read(TRGMII_7623_RD_0 + i * 8);
			err_cnt[i] = (tmp >> 8) & 0x0000000f;

			tmp = sys_reg_read(TRGMII_7623_RD_0 + i * 8);
			rd_wd = (tmp >> 16) & 0x000000ff;

			if (err_cnt[i] != 0)
				err_flag[i] = 1;
			else if (rd_wd != 0x55)
				err_flag[i] = 1;
			else
				err_flag[i] = 0;
			err_total_flag = err_flag[i] & err_total_flag;
		}

		/* Disable EDGE CHK in MT7623 */
		for (i = 0; i < 5; i++) {
			reg_bit_one(TRGMII_7623_RD_0 + i * 8, 30, 1);
			reg_bit_zero(TRGMII_7623_RD_0 + i * 8, 28, 2);
			reg_bit_zero(TRGMII_7623_RD_0 + i * 8, 31, 1);
		}
		wait_loop();
		/* Adjust RXC delay */
		/* RX clock gating in MT7623 */
		reg_bit_zero(TRGMII_7623_base + 0x04, 30, 2);
		read_data = sys_reg_read(TRGMII_7623_base);
		if (err_total_flag == 0) {
			tmp = (read_data & 0x0000007f) + rxc_step_size;
			read_data >>= 8;
			read_data &= 0xffffff80;
			read_data |= tmp;
			read_data <<= 8;
			read_data &= 0xffffff80;
			read_data |= tmp;
			sys_reg_write(TRGMII_7623_base, read_data);
		} else {
			tmp = (read_data & 0x0000007f) + 16;
			read_data >>= 8;
			read_data &= 0xffffff80;
			read_data |= tmp;
			read_data <<= 8;
			read_data &= 0xffffff80;
			read_data |= tmp;
			sys_reg_write(TRGMII_7623_base, read_data);
		}
		read_data &= 0x000000ff;
		/* Disable RX clock gating in MT7623 */
		reg_bit_one(TRGMII_7623_base + 0x04, 30, 2);
		for (i = 0; i < 5; i++)
			reg_bit_one(TRGMII_7623_RD_0 + i * 8, 31, 1);
	}
	/* Read RD_WD MT7623 */
	for (i = 0; i < 5; i++) {
		temp_addr = TRGMII_7623_RD_0 + i * 8;
		rd_tap = 0;
		while (err_flag[i] != 0 && rd_tap != 128) {
			/* Enable EDGE CHK in MT7623 */
			tmp = sys_reg_read(temp_addr);
			tmp |= 0x40000000;
			reg_bit_zero(temp_addr, 28, 4);
			reg_bit_one(temp_addr, 30, 1);
			wait_loop();
			read_data = sys_reg_read(temp_addr);
			/* Read MT7623 Errcnt */
			err_cnt[i] = (read_data >> 8) & 0x0000000f;
			rd_wd = (read_data >> 16) & 0x000000ff;
			if (err_cnt[i] != 0 || rd_wd != 0x55)
				err_flag[i] = 1;
			else
				err_flag[i] = 0;
			/* Disable EDGE CHK in MT7623 */
			reg_bit_zero(temp_addr, 28, 2);
			reg_bit_zero(temp_addr, 31, 1);
			tmp |= 0x40000000;
			sys_reg_write(temp_addr, tmp & 0x4fffffff);
			wait_loop();
			if (err_flag[i] != 0) {
				/* Add RXD delay in MT7623 */
				rd_tap = (read_data & 0x7f) + rxd_step_size;

				read_data = (read_data & 0xffffff80) | rd_tap;
				sys_reg_write(temp_addr, read_data);
				tap_a[i] = rd_tap;
			} else {
				rd_tap = (read_data & 0x0000007f) + 48;
				read_data = (read_data & 0xffffff80) | rd_tap;
				sys_reg_write(temp_addr, read_data);
			}
		}
		pr_info("MT7623 %dth bit  Tap_a = %d\n", i, tap_a[i]);
	}
	for (i = 0; i < 5; i++) {
		while ((err_flag[i] == 0) && (rd_tap != 128)) {
			read_data = sys_reg_read(TRGMII_7623_RD_0 + i * 8);
			/* Add RXD delay in MT7623 */
			rd_tap = (read_data & 0x7f) + rxd_step_size;

			read_data = (read_data & 0xffffff80) | rd_tap;
			sys_reg_write(TRGMII_7623_RD_0 + i * 8, read_data);

			/* Enable EDGE CHK in MT7623 */
			tmp = sys_reg_read(TRGMII_7623_RD_0 + i * 8);
			tmp |= 0x40000000;
			sys_reg_write(TRGMII_7623_RD_0 + i * 8,
				      (tmp & 0x4fffffff));
			wait_loop();
			read_data = sys_reg_read(TRGMII_7623_RD_0 + i * 8);
			/* Read MT7623 Errcnt */
			err_cnt[i] = (read_data >> 8) & 0xf;
			rd_wd = (read_data >> 16) & 0x000000ff;
			if (err_cnt[i] != 0 || rd_wd != 0x55)
				err_flag[i] = 1;
			else
				err_flag[i] = 0;

			/* Disable EDGE CHK in MT7623 */
			tmp = sys_reg_read(TRGMII_7623_RD_0 + i * 8);
			tmp |= 0x40000000;
			sys_reg_write(TRGMII_7623_RD_0 + i * 8,
				      (tmp & 0x4fffffff));
			wait_loop();
		}
		tap_b[i] = rd_tap;	/* -rxd_step_size; */
		pr_info("MT7623 %dth bit  Tap_b = %d\n", i, tap_b[i]);
		/* Calculate RXD delay = (TAP_A + TAP_B)/2 */
		final_tap[i] = (tap_a[i] + tap_b[i]) / 2;
		read_data = (read_data & 0xffffff80) | final_tap[i];
		sys_reg_write(TRGMII_7623_RD_0 + i * 8, read_data);
	}

	mii_mgr_read(0x1F, 0x7A40, &read_data);
	read_data &= 0x3fffffff;
	mii_mgr_write(0x1F, 0x7A40, read_data);
}

static void trgmii_calibration_7530(void)
{
	struct END_DEVICE *ei_local = netdev_priv(dev_raether);
	unsigned int tap_a[5] = {
		0, 0, 0, 0, 0
	};
	unsigned int tap_b[5] = {
		0, 0, 0, 0, 0
	};
	unsigned int final_tap[5];
	unsigned int rxc_step_size;
	unsigned int rxd_step_size;
	unsigned int read_data;
	unsigned int tmp = 0;
	int i;
	unsigned int err_cnt[5];
	unsigned int rd_wd;
	unsigned int init_toggle_data;
	unsigned int err_flag[5];
	unsigned int err_total_flag;
	unsigned int training_word;
	unsigned int rd_tap;

	void __iomem *TRGMII_7623_base;
	u32 TRGMII_7530_RD_0;
	u32 TRGMII_7530_base;
	u32 TRGMII_7530_TX_base;

	TRGMII_7623_base = ETHDMASYS_ETH_SW_BASE + 0x0300;
	TRGMII_7530_base = 0x7A00;
	TRGMII_7530_RD_0 = TRGMII_7530_base + 0x10;
	rxd_step_size = 0x1;
	rxc_step_size = 0x8;
	init_toggle_data = 0x00000055;
	training_word = 0x000000AC;

	TRGMII_7530_TX_base = TRGMII_7530_base + 0x50;

	reg_bit_one(TRGMII_7623_base + 0x40, 31, 1);
	mii_mgr_read(0x1F, 0x7a10, &read_data);

	/* RX clock gating in MT7530 */
	mii_mgr_read(0x1F, TRGMII_7530_base + 0x04, &read_data);
	read_data &= 0x3fffffff;
	mii_mgr_write(0x1F, TRGMII_7530_base + 0x04, read_data);

	/* Set TX OE edge in  MT7530 */
	mii_mgr_read(0x1F, TRGMII_7530_base + 0x78, &read_data);
	read_data |= 0x00002000;
	mii_mgr_write(0x1F, TRGMII_7530_base + 0x78, read_data);

	/* Assert RX  reset in MT7530 */
	mii_mgr_read(0x1F, TRGMII_7530_base, &read_data);
	read_data |= 0x80000000;
	mii_mgr_write(0x1F, TRGMII_7530_base, read_data);

	/* Release RX reset in MT7530 */
	mii_mgr_read(0x1F, TRGMII_7530_base, &read_data);
	read_data &= 0x7fffffff;
	mii_mgr_write(0x1F, TRGMII_7530_base, read_data);

	/* Disable RX clock gating in MT7530 */
	mii_mgr_read(0x1F, TRGMII_7530_base + 0x04, &read_data);
	read_data |= 0xC0000000;
	mii_mgr_write(0x1F, TRGMII_7530_base + 0x04, read_data);

	/*Enable Training Mode in MT7623 */
	reg_bit_zero(TRGMII_7623_base + 0x40, 30, 1);
	if (ei_local->architecture & GE1_TRGMII_FORCE_2000)
		reg_bit_one(TRGMII_7623_base + 0x40, 30, 2);
	else
		reg_bit_one(TRGMII_7623_base + 0x40, 31, 1);
	reg_bit_zero(TRGMII_7623_base + 0x78, 8, 4);
	reg_bit_zero(TRGMII_7623_base + 0x50, 8, 4);
	reg_bit_zero(TRGMII_7623_base + 0x58, 8, 4);
	reg_bit_zero(TRGMII_7623_base + 0x60, 8, 4);
	reg_bit_zero(TRGMII_7623_base + 0x68, 8, 4);
	reg_bit_zero(TRGMII_7623_base + 0x70, 8, 4);
	reg_bit_one(TRGMII_7623_base + 0x78, 11, 1);

	err_total_flag = 0;
	read_data = 0x0;
	while (err_total_flag == 0 && (read_data != 0x68)) {
		/* Enable EDGE CHK in MT7530 */
		for (i = 0; i < 5; i++) {
			mii_mgr_read(0x1F, TRGMII_7530_RD_0 + i * 8,
				     &read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F, TRGMII_7530_RD_0 + i * 8,
				      read_data);
			wait_loop();
			mii_mgr_read(0x1F, TRGMII_7530_RD_0 + i * 8,
				     &err_cnt[i]);
			err_cnt[i] >>= 8;
			err_cnt[i] &= 0x0000ff0f;
			rd_wd = err_cnt[i] >> 8;
			rd_wd &= 0x000000ff;
			err_cnt[i] &= 0x0000000f;
			if (err_cnt[i] != 0)
				err_flag[i] = 1;
			else if (rd_wd != 0x55)
				err_flag[i] = 1;
			else
				err_flag[i] = 0;

			if (i == 0)
				err_total_flag = err_flag[i];
			else
				err_total_flag = err_flag[i] & err_total_flag;
			/* Disable EDGE CHK in MT7530 */
			mii_mgr_read(0x1F, TRGMII_7530_RD_0 + i * 8,
				     &read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F, TRGMII_7530_RD_0 + i * 8,
				      read_data);
			wait_loop();
		}
		/*Adjust RXC delay */
		if (err_total_flag == 0) {
			/* Assert RX  reset in MT7530 */
			mii_mgr_read(0x1F, TRGMII_7530_base, &read_data);
			read_data |= 0x80000000;
			mii_mgr_write(0x1F, TRGMII_7530_base, read_data);

			/* RX clock gating in MT7530 */
			mii_mgr_read(0x1F, TRGMII_7530_base + 0x04, &read_data);
			read_data &= 0x3fffffff;
			mii_mgr_write(0x1F, TRGMII_7530_base + 0x04, read_data);

			mii_mgr_read(0x1F, TRGMII_7530_base, &read_data);
			tmp = read_data;
			tmp &= 0x0000007f;
			tmp += rxc_step_size;
			read_data &= 0xffffff80;
			read_data |= tmp;
			mii_mgr_write(0x1F, TRGMII_7530_base, read_data);
			mii_mgr_read(0x1F, TRGMII_7530_base, &read_data);

			/* Release RX reset in MT7530 */
			mii_mgr_read(0x1F, TRGMII_7530_base, &read_data);
			read_data &= 0x7fffffff;
			mii_mgr_write(0x1F, TRGMII_7530_base, read_data);

			/* Disable RX clock gating in MT7530 */
			mii_mgr_read(0x1F, TRGMII_7530_base + 0x04, &read_data);
			read_data |= 0xc0000000;
			mii_mgr_write(0x1F, TRGMII_7530_base + 0x04, read_data);
		}
		read_data = tmp;
	}
	/* Read RD_WD MT7530 */
	for (i = 0; i < 5; i++) {
		rd_tap = 0;
		while (err_flag[i] != 0 && rd_tap != 128) {
			/* Enable EDGE CHK in MT7530 */
			mii_mgr_read(0x1F, TRGMII_7530_RD_0 + i * 8,
				     &read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F, TRGMII_7530_RD_0 + i * 8,
				      read_data);
			wait_loop();
			err_cnt[i] = (read_data >> 8) & 0x0000000f;
			rd_wd = (read_data >> 16) & 0x000000ff;
			if (err_cnt[i] != 0 || rd_wd != 0x55)
				err_flag[i] = 1;
			else
				err_flag[i] = 0;

			if (err_flag[i] != 0) {
				/* Add RXD delay in MT7530 */
				rd_tap = (read_data & 0x7f) + rxd_step_size;
				read_data = (read_data & 0xffffff80) | rd_tap;
				mii_mgr_write(0x1F, TRGMII_7530_RD_0 + i * 8,
					      read_data);
				tap_a[i] = rd_tap;
			} else {
				/* Record the min delay TAP_A */
				tap_a[i] = (read_data & 0x0000007f);
				rd_tap = tap_a[i] + 0x4;
				read_data = (read_data & 0xffffff80) | rd_tap;
				mii_mgr_write(0x1F, TRGMII_7530_RD_0 + i * 8,
					      read_data);
			}

			/* Disable EDGE CHK in MT7530 */
			mii_mgr_read(0x1F, TRGMII_7530_RD_0 + i * 8,
				     &read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F, TRGMII_7530_RD_0 + i * 8,
				      read_data);
			wait_loop();
		}
		pr_info("MT7530 %dth bit  Tap_a = %d\n", i, tap_a[i]);
	}
	for (i = 0; i < 5; i++) {
		rd_tap = 0;
		while (err_flag[i] == 0 && (rd_tap != 128)) {
			/* Enable EDGE CHK in MT7530 */
			mii_mgr_read(0x1F, TRGMII_7530_RD_0 + i * 8,
				     &read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F, TRGMII_7530_RD_0 + i * 8,
				      read_data);
			wait_loop();
			err_cnt[i] = (read_data >> 8) & 0x0000000f;
			rd_wd = (read_data >> 16) & 0x000000ff;
			if (err_cnt[i] != 0 || rd_wd != 0x55)
				err_flag[i] = 1;
			else
				err_flag[i] = 0;

			if (err_flag[i] == 0 && (rd_tap != 128)) {
				/* Add RXD delay in MT7530 */
				rd_tap = (read_data & 0x7f) + rxd_step_size;
				read_data = (read_data & 0xffffff80) | rd_tap;
				mii_mgr_write(0x1F, TRGMII_7530_RD_0 + i * 8,
					      read_data);
			}
			/* Disable EDGE CHK in MT7530 */
			mii_mgr_read(0x1F, TRGMII_7530_RD_0 + i * 8,
				     &read_data);
			read_data |= 0x40000000;
			read_data &= 0x4fffffff;
			mii_mgr_write(0x1F, TRGMII_7530_RD_0 + i * 8,
				      read_data);
			wait_loop();
		}
		tap_b[i] = rd_tap;	/* - rxd_step_size; */
		pr_info("MT7530 %dth bit  Tap_b = %d\n", i, tap_b[i]);
		/* Calculate RXD delay = (TAP_A + TAP_B)/2 */
		final_tap[i] = (tap_a[i] + tap_b[i]) / 2;
		read_data = (read_data & 0xffffff80) | final_tap[i];
		mii_mgr_write(0x1F, TRGMII_7530_RD_0 + i * 8, read_data);
	}
	if (ei_local->architecture & GE1_TRGMII_FORCE_2000)
		reg_bit_zero(TRGMII_7623_base + 0x40, 31, 1);
	else
		reg_bit_zero(TRGMII_7623_base + 0x40, 30, 2);
}

static void mt7530_trgmii_clock_setting(u32 xtal_mode)
{
	u32 reg_value;
	/* TRGMII Clock */
	mii_mgr_write_cl45(0, 0x1f, 0x410, 0x1);
	if (xtal_mode == 1) {	/* 25MHz */
		mii_mgr_write_cl45(0, 0x1f, 0x404, MT7530_TRGMII_PLL_25M);
	} else if (xtal_mode == 2) {	/* 40MHz */
		mii_mgr_write_cl45(0, 0x1f, 0x404, MT7530_TRGMII_PLL_40M);
	}
	mii_mgr_write_cl45(0, 0x1f, 0x405, 0);
	if (xtal_mode == 1)	/* 25MHz */
		mii_mgr_write_cl45(0, 0x1f, 0x409, 0x57);
	else
		mii_mgr_write_cl45(0, 0x1f, 0x409, 0x87);

	if (xtal_mode == 1)	/* 25MHz */
		mii_mgr_write_cl45(0, 0x1f, 0x40a, 0x57);
	else
		mii_mgr_write_cl45(0, 0x1f, 0x40a, 0x87);

	mii_mgr_write_cl45(0, 0x1f, 0x403, 0x1800);
	mii_mgr_write_cl45(0, 0x1f, 0x403, 0x1c00);
	mii_mgr_write_cl45(0, 0x1f, 0x401, 0xc020);
	mii_mgr_write_cl45(0, 0x1f, 0x406, 0xa030);
	mii_mgr_write_cl45(0, 0x1f, 0x406, 0xa038);
	usleep_range(120, 130);		/* for MT7623 bring up test */
	mii_mgr_write_cl45(0, 0x1f, 0x410, 0x3);

	mii_mgr_read(31, 0x7830, &reg_value);
	reg_value &= 0xFFFFFFFC;
	reg_value |= 0x00000001;
	mii_mgr_write(31, 0x7830, reg_value);

	mii_mgr_read(31, 0x7a40, &reg_value);
	reg_value &= ~(0x1 << 30);
	reg_value &= ~(0x1 << 28);
	mii_mgr_write(31, 0x7a40, reg_value);

	mii_mgr_write(31, 0x7a78, 0x55);
	usleep_range(100, 110);		/* for mt7623 bring up test */

	/* Release MT7623 RXC reset */
	reg_bit_zero(ETHDMASYS_ETH_SW_BASE + 0x0300, 31, 1);

	trgmii_calibration_7623();
	trgmii_calibration_7530();
	/* Assert RX  reset in MT7623 */
	reg_bit_one(ETHDMASYS_ETH_SW_BASE + 0x0300, 31, 1);
	/* Release RX reset in MT7623 */
	reg_bit_zero(ETHDMASYS_ETH_SW_BASE + 0x0300, 31, 1);
	mii_mgr_read(31, 0x7a00, &reg_value);
	reg_value |= (0x1 << 31);
	mii_mgr_write(31, 0x7a00, reg_value);
	mdelay(1);
	reg_value &= ~(0x1 << 31);
	mii_mgr_write(31, 0x7a00, reg_value);
	mdelay(100);
}

static void is_switch_vlan_table_busy(void)
{
	int j = 0;
	unsigned int value = 0;

	for (j = 0; j < 20; j++) {
		mii_mgr_read(31, 0x90, &value);
		if ((value & 0x80000000) == 0) {	/* table busy */
			break;
		}
		mdelay(70);
	}
	if (j == 20)
		pr_info("set vlan timeout value=0x%x.\n", value);
}

static void lan_wan_partition(void)
{
	struct END_DEVICE *ei_local = netdev_priv(dev_raether);
	/*Set  MT7530 */
	if (ei_local->architecture & WAN_AT_P0) {
		pr_info("set LAN/WAN WLLLL\n");
		/*WLLLL, wan at P0 */
		/*LAN/WAN ports as security mode */
		mii_mgr_write(31, 0x2004, 0xff0003);	/* port0 */
		mii_mgr_write(31, 0x2104, 0xff0003);	/* port1 */
		mii_mgr_write(31, 0x2204, 0xff0003);	/* port2 */
		mii_mgr_write(31, 0x2304, 0xff0003);	/* port3 */
		mii_mgr_write(31, 0x2404, 0xff0003);	/* port4 */
		mii_mgr_write(31, 0x2504, 0xff0003);	/* port5 */
		mii_mgr_write(31, 0x2604, 0xff0003);	/* port6 */

		/*set PVID */
		mii_mgr_write(31, 0x2014, 0x10002);	/* port0 */
		mii_mgr_write(31, 0x2114, 0x10001);	/* port1 */
		mii_mgr_write(31, 0x2214, 0x10001);	/* port2 */
		mii_mgr_write(31, 0x2314, 0x10001);	/* port3 */
		mii_mgr_write(31, 0x2414, 0x10001);	/* port4 */
		mii_mgr_write(31, 0x2514, 0x10002);	/* port5 */
		mii_mgr_write(31, 0x2614, 0x10001);	/* port6 */
		/*port6 */
		/*VLAN member */
		is_switch_vlan_table_busy();
		mii_mgr_write(31, 0x94, 0x405e0001);	/* VAWD1 */
		mii_mgr_write(31, 0x90, 0x80001001);	/* VTCR, VID=1 */
		is_switch_vlan_table_busy();

		mii_mgr_write(31, 0x94, 0x40210001);	/* VAWD1 */
		mii_mgr_write(31, 0x90, 0x80001002);	/* VTCR, VID=2 */
		is_switch_vlan_table_busy();
	}
	if (ei_local->architecture & WAN_AT_P4) {
		pr_info("set LAN/WAN LLLLW\n");
		/*LLLLW, wan at P4 */
		/*LAN/WAN ports as security mode */
		mii_mgr_write(31, 0x2004, 0xff0003);	/* port0 */
		mii_mgr_write(31, 0x2104, 0xff0003);	/* port1 */
		mii_mgr_write(31, 0x2204, 0xff0003);	/* port2 */
		mii_mgr_write(31, 0x2304, 0xff0003);	/* port3 */
		mii_mgr_write(31, 0x2404, 0xff0003);	/* port4 */
		mii_mgr_write(31, 0x2504, 0xff0003);	/* port5 */
		mii_mgr_write(31, 0x2604, 0xff0003);	/* port6 */

		/*set PVID */
		mii_mgr_write(31, 0x2014, 0x10001);	/* port0 */
		mii_mgr_write(31, 0x2114, 0x10001);	/* port1 */
		mii_mgr_write(31, 0x2214, 0x10001);	/* port2 */
		mii_mgr_write(31, 0x2314, 0x10001);	/* port3 */
		mii_mgr_write(31, 0x2414, 0x10002);	/* port4 */
		mii_mgr_write(31, 0x2514, 0x10002);	/* port5 */
		mii_mgr_write(31, 0x2614, 0x10001);	/* port6 */

		/*VLAN member */
		is_switch_vlan_table_busy();
		mii_mgr_write(31, 0x94, 0x404f0001);	/* VAWD1 */
		mii_mgr_write(31, 0x90, 0x80001001);	/* VTCR, VID=1 */
		is_switch_vlan_table_busy();
		mii_mgr_write(31, 0x94, 0x40300001);	/* VAWD1 */
		mii_mgr_write(31, 0x90, 0x80001002);	/* VTCR, VID=2 */
		is_switch_vlan_table_busy();
	}
}

static void mt7530_phy_setting(void)
{
	u32 i;
	u32 reg_value;

	for (i = 0; i < 5; i++) {
		/* Disable EEE */
		mii_mgr_write_cl45(i, 0x7, 0x3c, 0);
		/* Enable HW auto downshift */
		mii_mgr_write(i, 31, 0x1);
		mii_mgr_read(i, 0x14, &reg_value);
		reg_value |= (1 << 4);
		mii_mgr_write(i, 0x14, reg_value);
		/* Increase SlvDPSready time */
		mii_mgr_write(i, 31, 0x52b5);
		mii_mgr_write(i, 16, 0xafae);
		mii_mgr_write(i, 18, 0x2f);
		mii_mgr_write(i, 16, 0x8fae);
		/* Incease post_update_timer */
		mii_mgr_write(i, 31, 0x3);
		mii_mgr_write(i, 17, 0x4b);
		/* Adjust 100_mse_threshold */
		mii_mgr_write_cl45(i, 0x1e, 0x123, 0xffff);
		/* Disable mcc */
		mii_mgr_write_cl45(i, 0x1e, 0xa6, 0x300);
	}
}

static void setup_internal_gsw(void)
{
	void __iomem *gpio_base_virt = ioremap(ETH_GPIO_BASE, 0x1000);
	struct END_DEVICE *ei_local = netdev_priv(dev_raether);
	u32 reg_value;
	u32 xtal_mode;
	u32 i;

	if (ei_local->architecture &
	    (GE1_TRGMII_FORCE_2000 | GE1_TRGMII_FORCE_2600))
		reg_bit_one(RALINK_SYSCTL_BASE + 0x2c, 11, 1);
	else
		reg_bit_zero(RALINK_SYSCTL_BASE + 0x2c, 11, 1);
	reg_bit_one(ETHDMASYS_ETH_SW_BASE + 0x0390, 1, 1);/* TRGMII mode */

	/*Hardware reset Switch */

	reg_bit_zero((void __iomem *)gpio_base_virt + 0x520, 1, 1);
	mdelay(1);
	reg_bit_one((void __iomem *)gpio_base_virt + 0x520, 1, 1);
	mdelay(100);

	/* Assert MT7623 RXC reset */
	reg_bit_one(ETHDMASYS_ETH_SW_BASE + 0x0300, 31, 1);
	/*For MT7623 reset MT7530 */
	reg_bit_one(RALINK_SYSCTL_BASE + 0x34, 2, 1);
	mdelay(1);
	reg_bit_zero(RALINK_SYSCTL_BASE + 0x34, 2, 1);
	mdelay(100);

	/* Wait for Switch Reset Completed */
	for (i = 0; i < 100; i++) {
		mdelay(10);
		mii_mgr_read(31, 0x7800, &reg_value);
		if (reg_value != 0) {
			pr_info("MT7530 Reset Completed!!\n");
			break;
		}
		if (i == 99)
			pr_err("MT7530 Reset Timeout!!\n");
	}

	for (i = 0; i <= 4; i++) {
		/*turn off PHY */
		mii_mgr_read(i, 0x0, &reg_value);
		reg_value |= (0x1 << 11);
		mii_mgr_write(i, 0x0, reg_value);
	}
	mii_mgr_write(31, 0x7000, 0x3);	/* reset switch */
	usleep_range(100, 110);

	/* (GE1, Force 1000M/FD, FC ON) */
	sys_reg_write(RALINK_ETH_SW_BASE + 0x100, 0x2105e33b);
	mii_mgr_write(31, 0x3600, 0x5e33b);
	mii_mgr_read(31, 0x3600, &reg_value);
	/* (GE2, Link down) */
	sys_reg_write(RALINK_ETH_SW_BASE + 0x200, 0x00008000);

	mii_mgr_read(31, 0x7804, &reg_value);
	reg_value &= ~(1 << 8);	/* Enable Port 6 */
	reg_value |= (1 << 6);	/* Disable Port 5 */
	reg_value |= (1 << 13);	/* Port 5 as GMAC, no Internal PHY */

	if (ei_local->architecture & GMAC2) {
		/*RGMII2=Normal mode */
		reg_bit_zero(RALINK_SYSCTL_BASE + 0x60, 15, 1);

		/*GMAC2= RGMII mode */
		reg_bit_zero(SYSCFG1, 14, 2);
		if (ei_local->architecture & GE2_RGMII_AN) {
			mii_mgr_write(31, 0x3500, 0x56300);
			/* (GE2, auto-polling) */
			sys_reg_write(RALINK_ETH_SW_BASE + 0x200, 0x21056300);
			reg_value |= (1 << 6);	/* disable MT7530 P5 */
			enable_auto_negotiate(ei_local);

		} else {
			/* MT7530 P5 Force 1000 */
			mii_mgr_write(31, 0x3500, 0x5e33b);
			/* (GE2, Force 1000) */
			sys_reg_write(RALINK_ETH_SW_BASE + 0x200, 0x2105e33b);
			reg_value &= ~(1 << 6);	/* enable MT7530 P5 */
			reg_value |= ((1 << 7) | (1 << 13) | (1 << 16));
			if (ei_local->architecture & WAN_AT_P0)
				reg_value |= (1 << 20);
			else
				reg_value &= ~(1 << 20);
		}
	}
	reg_value &= ~(1 << 5);
	reg_value |= (1 << 16);	/* change HW-TRAP */
	pr_info("change HW-TRAP to 0x%x\n", reg_value);
	mii_mgr_write(31, 0x7804, reg_value);
	mii_mgr_read(31, 0x7800, &reg_value);
	reg_value = (reg_value >> 9) & 0x3;
	if (reg_value == 0x3) {	/* 25Mhz Xtal */
		xtal_mode = 1;
		/*Do Nothing */
	} else if (reg_value == 0x2) {	/* 40Mhz */
		xtal_mode = 2;
		/* disable MT7530 core clock */
		mii_mgr_write_cl45(0, 0x1f, 0x410, 0x0);

		mii_mgr_write_cl45(0, 0x1f, 0x40d, 0x2020);
		mii_mgr_write_cl45(0, 0x1f, 0x40e, 0x119);
		mii_mgr_write_cl45(0, 0x1f, 0x40d, 0x2820);
		usleep_range(20, 30);	/* suggest by CD */
		mii_mgr_write_cl45(0, 0x1f, 0x410, 0x1);
	} else {
		xtal_mode = 3;
	 /*TODO*/}

	/* set MT7530 central align */
	mii_mgr_read(31, 0x7830, &reg_value);
	reg_value &= ~1;
	reg_value |= 1 << 1;
	mii_mgr_write(31, 0x7830, reg_value);

	mii_mgr_read(31, 0x7a40, &reg_value);
	reg_value &= ~(1 << 30);
	mii_mgr_write(31, 0x7a40, reg_value);

	reg_value = 0x855;
	mii_mgr_write(31, 0x7a78, reg_value);

	mii_mgr_write(31, 0x7b00, 0x104);	/* delay setting for 10/1000M */
	mii_mgr_write(31, 0x7b04, 0x10);	/* delay setting for 10/1000M */

	/*Tx Driving */
	mii_mgr_write(31, 0x7a54, 0x88);	/* lower GE1 driving */
	mii_mgr_write(31, 0x7a5c, 0x88);	/* lower GE1 driving */
	mii_mgr_write(31, 0x7a64, 0x88);	/* lower GE1 driving */
	mii_mgr_write(31, 0x7a6c, 0x88);	/* lower GE1 driving */
	mii_mgr_write(31, 0x7a74, 0x88);	/* lower GE1 driving */
	mii_mgr_write(31, 0x7a7c, 0x88);	/* lower GE1 driving */
	mii_mgr_write(31, 0x7810, 0x11);	/* lower GE2 driving */
	/*Set MT7623 TX Driving */
	sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x0354, 0x88);
	sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x035c, 0x88);
	sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x0364, 0x88);
	sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x036c, 0x88);
	sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x0374, 0x88);
	sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x037c, 0x88);

	/* Set GE2 driving and slew rate */
	if (ei_local->architecture & GE2_RGMII_AN)
		sys_reg_write((void __iomem *)gpio_base_virt + 0xf00, 0xe00);
	else
		sys_reg_write((void __iomem *)gpio_base_virt + 0xf00, 0xa00);
	/* set GE2 TDSEL */
	sys_reg_write((void __iomem *)gpio_base_virt + 0x4c0, 0x5);
	/* set GE2 TUNE */
	sys_reg_write((void __iomem *)gpio_base_virt + 0xed0, 0);

	mt7530_trgmii_clock_setting(xtal_mode);
	if (ei_local->architecture & GE1_RGMII_FORCE_1000) {
		sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x0350, 0x55);
		sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x0358, 0x55);
		sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x0360, 0x55);
		sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x0368, 0x55);
		sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x0370, 0x55);
		sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x0378, 0x855);
	}

	lan_wan_partition();
	mt7530_phy_setting();
	for (i = 0; i <= 4; i++) {
		/*turn on PHY */
		mii_mgr_read(i, 0x0, &reg_value);
		reg_value &= ~(0x1 << 11);
		mii_mgr_write(i, 0x0, reg_value);
	}

	mii_mgr_read(31, 0x7808, &reg_value);
	reg_value |= (3 << 16);	/* Enable INTR */
	mii_mgr_write(31, 0x7808, reg_value);

	iounmap(gpio_base_virt);
}

void setup_external_gsw(void)
{
	/* reduce RGMII2 PAD driving strength */
	reg_bit_zero(PAD_RGMII2_MDIO_CFG, 4, 2);
	/*enable MDIO */
	reg_bit_zero(RALINK_SYSCTL_BASE + 0x60, 12, 2);

	/*RGMII1=Normal mode */
	reg_bit_zero(RALINK_SYSCTL_BASE + 0x60, 14, 1);
	/*GMAC1= RGMII mode */
	reg_bit_zero(SYSCFG1, 12, 2);

	/* (GE1, Link down) */
	sys_reg_write(RALINK_ETH_SW_BASE + 0x100, 0x00008000);

	/*RGMII2=Normal mode */
	reg_bit_zero(RALINK_SYSCTL_BASE + 0x60, 15, 1);
	/*GMAC2= RGMII mode */
	reg_bit_zero(SYSCFG1, 14, 2);

	/* (GE2, Force 1000M/FD, FC ON) */
	sys_reg_write(RALINK_ETH_SW_BASE + 0x200, 0x2105e33b);

} int is_marvell_gigaphy(int ge)
{
	struct END_DEVICE *ei_local = netdev_priv(dev_raether);
	u32 phy_id0 = 0, phy_id1 = 0, phy_address;

	if (ei_local->architecture & GE1_RGMII_AN)
		phy_address = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR;
	else
		phy_address = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2;

	if (!mii_mgr_read(phy_address, 2, &phy_id0)) {
		pr_err("\n Read PhyID 1 is Fail!!\n");
		phy_id0 = 0;
	}
	if (!mii_mgr_read(phy_address, 3, &phy_id1)) {
		pr_err("\n Read PhyID 1 is Fail!!\n");
		phy_id1 = 0;
	}

	if ((phy_id0 == EV_MARVELL_PHY_ID0) && (phy_id1 == EV_MARVELL_PHY_ID1))
		return 1;
	return 0;
}

int is_vtss_gigaphy(int ge)
{
	struct END_DEVICE *ei_local = netdev_priv(dev_raether);
	u32 phy_id0 = 0, phy_id1 = 0, phy_address;

	if (ei_local->architecture & GE1_RGMII_AN)
		phy_address = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR;
	else
		phy_address = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2;

	if (!mii_mgr_read(phy_address, 2, &phy_id0)) {
		pr_err("\n Read PhyID 1 is Fail!!\n");
		phy_id0 = 0;
	}
	if (!mii_mgr_read(phy_address, 3, &phy_id1)) {
		pr_err("\n Read PhyID 1 is Fail!!\n");
		phy_id1 = 0;
	}

	if ((phy_id0 == EV_VTSS_PHY_ID0) && (phy_id1 == EV_VTSS_PHY_ID1))
		return 1;
	return 0;
}

void fe_sw_preinit(struct END_DEVICE *ei_local)
{
	struct device_node *np = ei_local->switch_np;
	struct platform_device *pdev = of_find_device_by_node(np);
	struct mtk_gsw *gsw;
	int ret;

	gsw = platform_get_drvdata(pdev);
	if (!gsw) {
		pr_err("Failed to get gsw\n");
		return;
	}

	regulator_set_voltage(gsw->supply, 1000000, 1000000);
	ret = regulator_enable(gsw->supply);
	if (ret)
		pr_err("Failed to enable mt7530 power: %d\n", ret);

	ret = devm_gpio_request(&pdev->dev, gsw->reset_pin,
				"mediatek,reset-pin");
		if (ret)
			dev_err(&pdev->dev, "fail to devm_gpio_request\n");

	gpio_direction_output(gsw->reset_pin, 0);
	usleep_range(1000, 1100);
	gpio_set_value(gsw->reset_pin, 1);
	mdelay(100);
	devm_gpio_free(&pdev->dev, gsw->reset_pin);
}

void fe_sw_init(void)
{
	struct END_DEVICE *ei_local = netdev_priv(dev_raether);
	unsigned int reg_value = 0;
	unsigned int i = 0;
	/* Case1: MT7623/MT7622 GE1 + GigaPhy */
	if (ei_local->architecture & GE1_RGMII_AN) {
		enable_auto_negotiate(ei_local);
		if (is_marvell_gigaphy(1)) {
			if (ei_local->features & FE_FPGA_MODE) {
				mii_mgr_read(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR, 9,
					     &reg_value);
				/* turn off 1000Base-T Advertisement
				 * (9.9=1000Full, 9.8=1000Half)
				 */
				reg_value &= ~(3 << 8);
				mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR,
					      9, reg_value);

				/*10Mbps, debug */
				mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR,
					      4, 0x461);

				mii_mgr_read(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR, 0,
					     &reg_value);
				reg_value |= 1 << 9;	/* restart AN */
				mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR,
					      0, reg_value);
			}
		}
		if (is_vtss_gigaphy(1)) {
			mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR, 31, 1);
			mii_mgr_read(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR, 28,
				     &reg_value);
			pr_info("Vitesse phy skew: %x --> ", reg_value);
			reg_value |= (0x3 << 12);
			reg_value &= ~(0x3 << 14);
			pr_info("%x\n", reg_value);
			mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR, 28,
				      reg_value);
			mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR, 31, 0);
		}
	}

	/* Case2: RT3883/MT7621 GE2 + GigaPhy */
	if (ei_local->architecture & GE2_RGMII_AN) {
		enable_auto_negotiate(ei_local);
		if (is_marvell_gigaphy(2)) {
			mii_mgr_read(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2, 9,
				     &reg_value);
			/* turn off 1000Base-T Advertisement
			 * (9.9=1000Full, 9.8=1000Half)
			 */
			reg_value &= ~(3 << 8);
			mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2, 9,
				      reg_value);

			mii_mgr_read(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2, 20,
				     &reg_value);
			/* Add delay to RX_CLK for RXD Outputs */
			reg_value |= 1 << 7;
			mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2, 20,
				      reg_value);

			mii_mgr_read(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2, 0,
				     &reg_value);
			reg_value |= 1 << 15;	/* PHY Software Reset */
			mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2, 0,
				      reg_value);
			if (ei_local->features & FE_FPGA_MODE) {
				mii_mgr_read(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2,
					     9, &reg_value);
				/* turn off 1000Base-T Advertisement
				 * (9.9=1000Full, 9.8=1000Half)
				 */
				reg_value &= ~(3 << 8);
				mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2,
					      9, reg_value);

				/*10Mbps, debug */
				mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2,
					      4, 0x461);

				mii_mgr_read(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2,
					     0, &reg_value);
				reg_value |= 1 << 9;	/* restart AN */
				mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2,
					      0, reg_value);
			}
		}
		if (is_vtss_gigaphy(2)) {
			mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2, 31, 1);
			mii_mgr_read(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2, 28,
				     &reg_value);
			pr_info("Vitesse phy skew: %x --> ", reg_value);
			reg_value |= (0x3 << 12);
			reg_value &= ~(0x3 << 14);
			pr_info("%x\n", reg_value);
			mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2, 28,
				      reg_value);
			mii_mgr_write(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2, 31, 0);
		}
	}

	/* Case3:  MT7623 GE1 + Internal GigaSW */
	if (ei_local->architecture &
	    (GE1_RGMII_FORCE_1000 | GE1_TRGMII_FORCE_2000 |
	     GE1_TRGMII_FORCE_2600)) {
		if (ei_local->chip_name == MT7623_FE)
			setup_internal_gsw();
		/* TODO
		  * else if (ei_local->features & FE_FPGA_MODE)
		  * setup_fpga_gsw();
		  * else
		  * sys_reg_write(MDIO_CFG, INIT_VALUE_OF_FORCE_1000_FD);
		  */
	}

	/* Case4: MT7623 GE2 + GigaSW */
	if (ei_local->architecture & GE2_RGMII_FORCE_1000)
		setup_external_gsw();
	/*TODO
	  * else
	  * sys_reg_write(MDIO_CFG2, INIT_VALUE_OF_FORCE_1000_FD);
	  */

	/* Case5: MT7622 embedded switch */
	if (ei_local->architecture & RAETH_ESW) {
		pr_info("=====MT7622 Embedded Switch Init=====\n");
		/* TODO
		 * sys_reg_write(ETHDMASYS_ETH_MAC_BASE + 0xC, 0x1);
		 */
		sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x84, 0xffc01f00);
		sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0x90, 0x7f7f);
		sys_reg_write(ETHDMASYS_ETH_SW_BASE + 0xc8, 0x05503f38);

		for (i = 0; i < 5; i++) {
			/* reset all digital logic, except phy_reg */
			mii_mgr_write(i, 9, 0);
			/* Capable of 10M Full/Half Duplex,
			 * flow control on/off
			 */
			mii_mgr_write(i, 4, 0x0461);
			/* reset all digital logic, except phy_reg */
			mii_mgr_write(i, 0, 0x1340);
		}
	}

	if (ei_local->architecture & MT7622_EPHY)
		mt7622_ephy_cal();
}

void gsw_delay_setting(void)
{
	struct END_DEVICE *ei_local = netdev_priv(dev_raether);
	int reg_int_val = 0;
	int link_speed = 0;

	if ((ei_local->architecture & (GMAC2 | WAN_AT_P0)) ||
	    (ei_local->architecture & (GMAC2 | WAN_AT_P4))) {
		reg_int_val = sys_reg_read(FE_INT_STATUS2);
		if (reg_int_val & BIT(25)) {
			/* if link up */
			if (sys_reg_read(RALINK_ETH_SW_BASE + 0x0208) & 0x1) {
				link_speed =
				    (sys_reg_read(RALINK_ETH_SW_BASE + 0x0208)
				     >> 2 & 0x3);
				if (link_speed == 1) {
					/* delay setting for 100M */
					pr_info
					    ("MT7623 GE2 link rate to 100M\n");
				} else if (link_speed == 0) {
					/* delay setting for 10M */
					pr_info("MT7623 GE2 link rate to 10M\n");
				} else if (link_speed == 2) {
					/* delay setting for 1G */
					pr_info("MT7623 GE2 link rate to 1G\n");
				}
			}
		}
		sys_reg_write(FE_INT_STATUS2, reg_int_val);
	}
}

static void esw_link_status_changed(int port_no, void *dev_id)
{
	unsigned int reg_val;

	mii_mgr_read(31, (0x3008 + (port_no * 0x100)), &reg_val);
	if (reg_val & 0x1)
		pr_info("ESW: Link Status Changed - Port%d Link UP\n", port_no);
	else
		pr_info("ESW: Link Status Changed - Port%d Link Down\n",
			port_no);
}

irqreturn_t esw_interrupt(int irq, void *resv)
{
	unsigned long flags;
	unsigned int reg_int_val;
	struct net_device *dev = dev_raether;
	struct END_DEVICE *ei_local = netdev_priv(dev);
	void *dev_id = NULL;

	spin_lock_irqsave(&ei_local->page_lock, flags);
	mii_mgr_read(31, 0x700c, &reg_int_val);

	if (reg_int_val & P4_LINK_CH)
		esw_link_status_changed(4, dev_id);

	if (reg_int_val & P3_LINK_CH)
		esw_link_status_changed(3, dev_id);
	if (reg_int_val & P2_LINK_CH)
		esw_link_status_changed(2, dev_id);
	if (reg_int_val & P1_LINK_CH)
		esw_link_status_changed(1, dev_id);
	if (reg_int_val & P0_LINK_CH)
		esw_link_status_changed(0, dev_id);

	mii_mgr_write(31, 0x700c, 0x1f);	/* ack switch link change */
	spin_unlock_irqrestore(&ei_local->page_lock, flags);

	return IRQ_HANDLED;
}

static const struct of_device_id mediatek_gsw_match[] = {
	{ .compatible = "mediatek,mt7623-gsw" },
	{},
};
MODULE_DEVICE_TABLE(of, mediatek_gsw_match);

static int mtk_gsw_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device_node *pctl;
	struct mtk_gsw *gsw;

	gsw = devm_kzalloc(&pdev->dev, sizeof(struct mtk_gsw), GFP_KERNEL);
	if (!gsw)
		return -ENOMEM;

	gsw->dev = &pdev->dev;
	gsw->trgmii_force = 2000;
	gsw->irq = irq_of_parse_and_map(np, 0);
	if (gsw->irq < 0)
		return -EINVAL;

	gsw->ethsys = syscon_regmap_lookup_by_phandle(np, "mediatek,ethsys");
	if (IS_ERR(gsw->ethsys)) {
		pr_err("fail at %s %d\n", __func__, __LINE__);
		return PTR_ERR(gsw->ethsys);
	}

	gsw->reset_pin = of_get_named_gpio(np, "mediatek,reset-pin", 0);
	if (gsw->reset_pin < 0) {
		pr_err("fail at %s %d\n", __func__, __LINE__);
		return -1;
	}
	pr_debug("reset_pin_port= %d\n", gsw->reset_pin);

	pctl = of_parse_phandle(np, "mediatek,pctl-regmap", 0);
	if (IS_ERR(pctl)) {
		pr_err("fail at %s %d\n", __func__, __LINE__);
		return PTR_ERR(pctl);
	}

	gsw->pctl = syscon_node_to_regmap(pctl);
	if (IS_ERR(pctl)) {
		pr_err("fail at %s %d\n", __func__, __LINE__);
		return PTR_ERR(pctl);
	}

	gsw->pins = pinctrl_get(&pdev->dev);
		if (gsw->pins) {
			gsw->ps_reset =
				pinctrl_lookup_state(gsw->pins, "reset");

		if (IS_ERR(gsw->ps_reset)) {
			dev_err(&pdev->dev,
				"failed to lookup the gsw_reset state\n");
			return PTR_ERR(gsw->ps_reset);
		}
	} else {
		dev_err(&pdev->dev, "gsw get pinctrl fail\n");
		return PTR_ERR(gsw->pins);
	}

	gsw->supply = devm_regulator_get(&pdev->dev, "mt7530");
	if (IS_ERR(gsw->supply)) {
		pr_err("fail at %s %d\n", __func__, __LINE__);
		return PTR_ERR(gsw->supply);
	}

	gsw->wllll = of_property_read_bool(np, "mediatek,wllll");

	platform_set_drvdata(pdev, gsw);

	return 0;
}

static int mtk_gsw_remove(struct platform_device *pdev)
{
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver gsw_driver = {
	.probe = mtk_gsw_probe,
	.remove = mtk_gsw_remove,
	.driver = {
		.name = "mtk-gsw",
		.owner = THIS_MODULE,
		.of_match_table = mediatek_gsw_match,
	},
};

module_platform_driver(gsw_driver);


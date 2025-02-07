/*
* Copyright (C) 2016 MediaTek Inc.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See http://www.gnu.org/licenses/gpl-2.0.html for more details.
*/
 
/*
 *
 * Filename:
 * ---------
 *    mtk_charger.c
 *
 * Project:
 * --------
 *   Android_Software
 *
 * Description:
 * ------------
 *   This Module defines functions of Battery charging
 *
 * Author:
 * -------
 * Wy Chuang
 *
 */
#include <linux/init.h>		/* For init/exit macros */
#include <linux/module.h>	/* For MODULE_ marcros  */
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/power_supply.h>
#include <linux/wakelock.h>
#include <linux/time.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/proc_fs.h>
#include <linux/platform_device.h>
#include <linux/seq_file.h>
#include <linux/scatterlist.h>
#include <linux/suspend.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/reboot.h>

#include "mtk_charger_intf.h"
#include <mt-plat/charger_type.h>
#include <mt-plat/mtk_battery.h>
#include <mt-plat/mtk_boot.h>
#include <pmic.h>
#include <mtk_gauge_time_service.h>


#ifdef VENDOR_EDIT
/* Qiao.Hu@EXP.BSP.CHG.basic, 2017/07/20, Add for charger */
#include <soc/oppo/device_info.h>
#include <soc/oppo/oppo_project.h>
#include <linux/gpio.h>
#include "../../oppo/oppo_vooc.h"
#include "../../oppo/oppo_gauge.h"
#include "../../oppo/oppo_charger.h"
#include "../../oppo/charger_ic/op_charge.h"
#include <linux/of_gpio.h>

#ifdef ODM_WT_EDIT
//Mingyao.Xie@ODM_WT.BSP.Storage.Usb, 2018/08/07, Modify for USB
#include <soc/oppo/oppo_project.h>
extern unsigned int is_project(OPPO_PROJECT project );
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/12, add for usbtemp*/
#include <linux/iio/consumer.h>
#endif /*ODM_WT_EDIT*/

#endif  /* VENDOR_EDIT */
#ifdef VENDOR_EDIT
/* Qiao.Hu@EXP.BSP.BaseDrv.CHG.Basic, 2017/08/15, Add for charger full status */
extern bool oppo_chg_check_chip_is_null(void);
#endif /* VENDOR_EDIT */
#ifdef VENDOR_EDIT
/* Qiao.Hu@EXP.BSP.BaseDrv.CHG.Basic, 2017/08/15, Add for charger full status */
extern bool oppo_chg_check_chip_is_null(void);
#endif /* VENDOR_EDIT */

#ifdef VENDOR_EDIT
/************ kpoc_charger *******************/
//huangtongfeng@BSP.CHG.Basic, 2017/12/14, add for kpoc charging param.
extern int oppo_chg_get_ui_soc(void);
extern int oppo_chg_get_notify_flag(void);
extern int oppo_chg_show_vooc_logo_ornot(void);
extern bool pmic_chrdet_status(void);
extern int oppo_get_prop_status(void);
struct oppo_chg_chip *g_oppo_chip = NULL;
int oppo_usb_switch_gpio_gpio_init(void);
static bool oppo_ship_check_is_gpio(struct oppo_chg_chip *chip);
int oppo_ship_gpio_init(struct oppo_chg_chip *chip);
void smbchg_enter_shipmode(struct oppo_chg_chip *chip);
bool oppo_shortc_check_is_gpio(struct oppo_chg_chip *chip);
int oppo_shortc_gpio_init(struct oppo_chg_chip *chip);
extern struct oppo_chg_operations  bq24190_chg_ops;
extern struct oppo_chg_operations  bq25890h_chg_ops;
extern struct oppo_chg_operations  bq25601d_chg_ops;
#ifdef ODM_WT_EDIT
/*Sidong.Zhao@ODM_WT.BSP.CHG 2019/10/26, Add rt9471 charger ops*/
extern struct oppo_chg_operations  oppo_chg_rt9471_ops;
extern struct oppo_chg_operations  oppo_chg_rt9467_ops;
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/12, add for usbtemp*/
struct iio_channel *usb_chan1; //usb_temp_auxadc_channel 1
struct iio_channel *usb_chan2; //usb_temp_auxadc_channel 2
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/12/04, add for chg debug*/
static int usb_debug_temp = 65535;
int ap_temp_debug = 65535;
module_param(usb_debug_temp, int, 0644);
module_param(ap_temp_debug, int, 0644);
#endif /*ODM_WT_EDIT*/
#ifdef CONFIG_OPPO_CHARGER_MTK6771
extern struct oppo_chg_operations  smb1351_chg_ops;
#endif
#ifdef CONFIG_OPPO_CHARGER_MT6370_TYPEC
/* Jianchao.Shi@BSP.CHG.Basic, 2019/06/10, sjc Add for charging */
extern struct oppo_chg_operations mt6370_chg_ops;
extern int oppo_get_typec_cc_orientation(void);
#endif
extern struct oppo_chg_operations * oppo_get_chg_ops(void);

extern bool oppo_gauge_ic_chip_is_null(void);
extern bool oppo_vooc_check_chip_is_null(void);
extern bool oppo_adapter_check_chip_is_null(void);
int oppo_tbatt_power_off_task_init(struct oppo_chg_chip *chip);

#define CHG_BQ25601D 2
#define CHG_BQ25890H 1
#define CHG_BQ24190  0
#define CHG_SMB1351  3
#define CHG_MT6370	4
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/11, add for typec_cc_orientation node*/
extern int oppo_get_typec_cc_orientation(void);
#endif /*ODM_WT_EDIT*/
static struct task_struct *oppo_usbtemp_kthread;
static DECLARE_WAIT_QUEUE_HEAD(oppo_usbtemp_wq);
void oppo_set_otg_switch_status(bool value);
void oppo_wake_up_usbtemp_thread(void);
//====================================================================//
#ifdef VENDOR_EDIT
/* Jianchao.Shi@BSP.CHG.Basic, 2019/07/25, sjc Add for usb status */
#define USB_TEMP_HIGH		0x01//bit0
#define USB_WATER_DETECT	0x02//bit1
#define USB_RESERVE2		0x04//bit2
#define USB_RESERVE3		0x08//bit3
#define USB_RESERVE4		0x10//bit4
#define USB_DONOT_USE		0x80000000//bit31
static int usb_status = 0;
static void oppo_set_usb_status(int status)
{
	usb_status = usb_status | status;
}
//static void oppo_clear_usb_status(int status)
//{
//	usb_status = usb_status & (~status);
//}

static int oppo_get_usb_status(void)
{
	return usb_status;
}
#endif /* VENDOR_EDIT */
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/12/07, add for ship mode function*/
extern int rt9471_enable_shipmode(bool en);
extern int rt9467_enable_shipmode(bool en);
void charger_ic_enable_ship_mode(void)
{
	if(is_project(19741))
		rt9471_enable_shipmode(true);
	else if(is_project(19747)){
		rt9471_enable_shipmode(true);
		rt9467_enable_shipmode(true);
	}
}
#endif /*ODM_WT_EDIT*/
//====================================================================//


int oppo_battery_meter_get_battery_voltage(void)
{
	//return battery_get_bat_voltage();
	return 4000;
}
#endif /* VENDOR_EDIT */


#ifdef VENDOR_EDIT
/* Qiao.Hu@BSP.BaseDrv.CHG.Basic, 2018/01/11, mtk patch for distinguish fast charging and normal charging*/
bool is_vooc_project(void)
{
	if (is_project(OPPO_17197) || is_project(OPPO_18531) || is_project(OPPO_18561) || (is_project(OPPO_18311) && get_Operator_Version() != OPERATOR_18328_ASIA_SIMPLE_NORMALCHG)
		|| is_project(OPPO_19391) || is_project(OPPO_19531)) {
		return true;
	} else {
		return false;
    }
}
#endif /* VENDOR_EDIT */

#ifdef VENDOR_EDIT
/* Qiao.Hu@BSP.BaseDrv.CHG.Basic, 2017/11/19, Add for charging */
bool meter_fg_30_get_battery_authenticate(void);
#endif /* VENDOR_EDIT */
#ifdef VENDOR_EDIT
/* Qiao.Hu@BSP.BaseDrv.CHG.Basic, 2017/11/19, Add for charging */
int charger_ic_flag = 1;
int oppo_which_charger_ic(void)
{
    return charger_ic_flag;
}
#endif /* VENDOR_EDIT */


#ifdef VENDOR_EDIT
/* Jianchao.Shi@BSP.CHG.Basic, 2017/01/22, sjc Add for charging*/
static int oppo_chg_parse_custom_dt(struct oppo_chg_chip *chip)
{
	int rc = 0;
	struct device_node *node = NULL;

	if (chip)
		node = chip->dev->of_node;
	if (!node) {
			pr_err("device tree node missing\n");
			return -EINVAL;
	}

#ifdef VENDOR_EDIT
/* Jianchao.Shi@BSP.CHG.Basic, 2017/01/22, sjc Add for charging*/
	if (chip) {
		chip->normalchg_gpio.chargerid_switch_gpio =
				of_get_named_gpio(node, "qcom,chargerid_switch-gpio", 0);
		if (chip->normalchg_gpio.chargerid_switch_gpio <= 0) {
			chg_err("Couldn't read chargerid_switch-gpio rc = %d, chargerid_switch_gpio:%d\n",
					rc, chip->normalchg_gpio.chargerid_switch_gpio);
		} else {
			if (gpio_is_valid(chip->normalchg_gpio.chargerid_switch_gpio)) {
				rc = gpio_request(chip->normalchg_gpio.chargerid_switch_gpio, "charging-switch1-gpio");
				if (rc) {
					chg_err("unable to request chargerid_switch_gpio:%d\n", chip->normalchg_gpio.chargerid_switch_gpio);
				} else {
					//smbchg_chargerid_switch_gpio_init(chip);
                    oppo_usb_switch_gpio_gpio_init();
				}
			}
			chg_err("chargerid_switch_gpio:%d\n", chip->normalchg_gpio.chargerid_switch_gpio);
		}
	}
#endif /*VENDOR_EDIT*/

#ifdef VENDOR_EDIT
/* Jianchao.Shi@BSP.CHG.Basic, 2018/03/02, sjc Add for using gpio as shipmode stm6620 */
	if (chip) {
		chip->normalchg_gpio.ship_gpio =
				of_get_named_gpio(node, "qcom,ship-gpio", 0);
		if (chip->normalchg_gpio.ship_gpio <= 0) {
			chg_err("Couldn't read qcom,ship-gpio rc = %d, qcom,ship-gpio:%d\n",
					rc, chip->normalchg_gpio.ship_gpio);
		} else {
			if (oppo_ship_check_is_gpio(chip) == true) {
				rc = gpio_request(chip->normalchg_gpio.ship_gpio, "ship-gpio");
				if (rc) {
					chg_err("unable to request ship-gpio:%d\n", chip->normalchg_gpio.ship_gpio);
				} else {
					oppo_ship_gpio_init(chip);
					if (rc)
						chg_err("unable to init ship-gpio:%d\n", chip->normalchg_gpio.ship_gpio);
				}
			}
			chg_err("ship-gpio:%d\n", chip->normalchg_gpio.ship_gpio);
		}
	}
#endif /*VENDOR_EDIT*/

#ifdef VENDOR_EDIT
/* Jianchao.Shi@BSP.CHG.Basic, 2018/03/02, sjc Add for HW shortc */
	if (chip) {
		chip->normalchg_gpio.shortc_gpio =
				of_get_named_gpio(node, "qcom,shortc-gpio", 0);
		if (chip->normalchg_gpio.shortc_gpio <= 0) {
			chg_err("Couldn't read qcom,shortc-gpio rc = %d, qcom,shortc-gpio:%d\n",
					rc, chip->normalchg_gpio.shortc_gpio);
		} else {
			if (oppo_shortc_check_is_gpio(chip) == true) {
				rc = gpio_request(chip->normalchg_gpio.shortc_gpio, "shortc-gpio");
				if (rc) {
					chg_err("unable to request shortc-gpio:%d\n", chip->normalchg_gpio.shortc_gpio);
				} else {
					oppo_shortc_gpio_init(chip);
					if (rc)
						chg_err("unable to init ship-gpio:%d\n", chip->normalchg_gpio.ship_gpio);
				}
			}
			chg_err("shortc-gpio:%d\n", chip->normalchg_gpio.shortc_gpio);
		}
	}
#endif /* VENDOR_EDIT */

	return rc;
}
#endif /*VENDOR_EDIT*/

#ifdef VENDOR_EDIT
extern enum charger_type mt_charger_type_detection(void);
extern enum charger_type mt_charger_type_detection_bq25890h(void);
extern enum charger_type mt_charger_type_detection_bq25601d(void);
#ifdef CONFIG_OPPO_CHARGER_MTK6771
extern int smb1351_get_charger_type(void);
#endif
extern bool upmu_is_chr_det(void);
extern enum charger_type g_chr_type;
//extern int otg_is_exist;

#ifdef VENDOR_EDIT//Qiao.Hu@BSP.BaseDrv.CHG.Basic,add 2017/12/09 for shipmode  stm6620


static bool oppo_ship_check_is_gpio(struct oppo_chg_chip *chip)
{
	if (gpio_is_valid(chip->normalchg_gpio.ship_gpio))
		return true;

	return false;
}

int oppo_ship_gpio_init(struct oppo_chg_chip *chip)
{
	chip->normalchg_gpio.pinctrl = devm_pinctrl_get(chip->dev);
	chip->normalchg_gpio.ship_active =
		pinctrl_lookup_state(chip->normalchg_gpio.pinctrl,
			"ship_active");

	if (IS_ERR_OR_NULL(chip->normalchg_gpio.ship_active)) {
		chg_err("get ship_active fail\n");
		return -EINVAL;
	}
	chip->normalchg_gpio.ship_sleep =
			pinctrl_lookup_state(chip->normalchg_gpio.pinctrl,
				"ship_sleep");
	if (IS_ERR_OR_NULL(chip->normalchg_gpio.ship_sleep)) {
		chg_err("get ship_sleep fail\n");
		return -EINVAL;
	}

	pinctrl_select_state(chip->normalchg_gpio.pinctrl,
		chip->normalchg_gpio.ship_sleep);
	return 0;
}

#define SHIP_MODE_CONFIG		0x40
#define SHIP_MODE_MASK			BIT(0)
#define SHIP_MODE_ENABLE		0
#define PWM_COUNT				5
void smbchg_enter_shipmode(struct oppo_chg_chip *chip)
{
	int i = 0;
	chg_err("enter smbchg_enter_shipmode\n");

	if (oppo_ship_check_is_gpio(chip) == true) {
		chg_err("select gpio control\n");
		if (!IS_ERR_OR_NULL(chip->normalchg_gpio.ship_active) && !IS_ERR_OR_NULL(chip->normalchg_gpio.ship_sleep)) {
			pinctrl_select_state(chip->normalchg_gpio.pinctrl,
				chip->normalchg_gpio.ship_sleep);
			for (i = 0; i < PWM_COUNT; i++) {
				//gpio_direction_output(chip->normalchg_gpio.ship_gpio, 1);
				pinctrl_select_state(chip->normalchg_gpio.pinctrl, chip->normalchg_gpio.ship_active);
				mdelay(3);
				//gpio_direction_output(chip->normalchg_gpio.ship_gpio, 0);
				pinctrl_select_state(chip->normalchg_gpio.pinctrl, chip->normalchg_gpio.ship_sleep);
				mdelay(3);
			}
		}
		chg_err("power off after 15s\n");
	}
}
void enter_ship_mode_function(struct oppo_chg_chip *chip)
{
	if(chip != NULL){
		if (chip->enable_shipmode) {
			printk("enter_ship_mode_function\n");
			smbchg_enter_shipmode(chip);
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/12/07, add for ship mode function*/
			charger_ic_enable_ship_mode();
#endif /*ODM_WT_EDIT*/
		}
	}
}

#endif /* VENDOR_EDIT */

#ifdef VENDOR_EDIT
//tongfeng.Huang@BSP.BaseDrv.CHG.Basic,add 2018/02/09 for short c hw check;
bool oppo_shortc_check_is_gpio(struct oppo_chg_chip *chip)
{
	if (gpio_is_valid(chip->normalchg_gpio.shortc_gpio))
	{
		return true;
	}
	return false;
}

int oppo_shortc_gpio_init(struct oppo_chg_chip *chip)
{
	chip->normalchg_gpio.pinctrl = devm_pinctrl_get(chip->dev);
	chip->normalchg_gpio.shortc_active =
		pinctrl_lookup_state(chip->normalchg_gpio.pinctrl,
			"shortc_active");

	if (IS_ERR_OR_NULL(chip->normalchg_gpio.shortc_active)) {
		chg_err("get shortc_active fail\n");
		return -EINVAL;
        }

	pinctrl_select_state(chip->normalchg_gpio.pinctrl,
		chip->normalchg_gpio.shortc_active);
	return 0;
}
#ifdef CONFIG_OPPO_SHORT_HW_CHECK
bool oppo_chg_get_shortc_hw_gpio_status(void)
{
	bool shortc_hw_status = 1;

	if(oppo_shortc_check_is_gpio(g_oppo_chip) == true) {
		shortc_hw_status = !!(gpio_get_value(g_oppo_chip->normalchg_gpio.shortc_gpio));
	}
	return shortc_hw_status;
}
#else
bool oppo_chg_get_shortc_hw_gpio_status(void)
{
	bool shortc_hw_status = 1;

	return shortc_hw_status;
}
#endif
int oppo_chg_shortc_hw_parse_dt(struct oppo_chg_chip *chip)
{
	int rc = 0;
	struct device_node *node = NULL;

	if (chip)
		node = chip->dev->of_node;
	if (!node) {
		dev_err(chip->dev, "device tree info. missing\n");
		return -EINVAL;
	}

	if (chip) {
		chip->normalchg_gpio.shortc_gpio = of_get_named_gpio(node, "qcom,shortc_gpio", 0);
		if (chip->normalchg_gpio.shortc_gpio <= 0) {
			chg_err("Couldn't read qcom,shortc_gpio rc = %d, qcom,shortc_gpio:%d\n",
			rc, chip->normalchg_gpio.shortc_gpio);
		} else {
			if(oppo_shortc_check_is_gpio(chip) == true) {
				chg_debug("This project use gpio for shortc hw check\n");
				rc = gpio_request(chip->normalchg_gpio.shortc_gpio, "shortc_gpio");
				if(rc){
					chg_err("unable to request shortc_gpio:%d\n",
					chip->normalchg_gpio.shortc_gpio);
				} else {
					oppo_shortc_gpio_init(chip);
				}
			} else {
				chg_err("chip->normalchg_gpio.shortc_gpio is not valid or get_PCB_Version() < V0.3:%d\n",
				get_PCB_Version());
			}
			chg_err("shortc_gpio:%d\n", chip->normalchg_gpio.shortc_gpio);
		}
	}
	return rc;
}
#endif

#ifndef ODM_WT_EDIT
/*Sidong.Zhao@ODM_WT.BSP.CHG 2019/10/26, Add rt9471 charger ops*/
int mt_power_supply_type_check(void)
{
	int charger_type = POWER_SUPPLY_TYPE_UNKNOWN;
	int charger_type_final = 0;
	/*if(otg_is_exist == 1)
		return g_chr_type;
	chg_debug("mt_power_supply_type_check-----1---------charger_type = %d\r\n",charger_type);*/
	if(true == upmu_is_chr_det()) {
		if(CHG_BQ24190 == charger_ic_flag) {
			charger_type_final = mt_charger_type_detection();
		} else if(CHG_BQ25890H == charger_ic_flag){
			charger_type_final = mt_charger_type_detection_bq25890h();
            //charger_type_final = STANDARD_HOST;
		} else if (CHG_BQ25601D == charger_ic_flag) {
			charger_type_final = mt_charger_type_detection_bq25601d();
		} else if (CHG_SMB1351 == charger_ic_flag) {
			#ifdef CONFIG_OPPO_CHARGER_MTK6771
			  charger_type_final = smb1351_get_charger_type();
			#else
			  chg_err("Un supported charger type");
			#endif
		}
		g_chr_type = charger_type_final;
	}
	else {
		chg_debug(" call first type\n");
		charger_type_final = g_chr_type;
	}

	switch(charger_type_final) {
	case CHARGER_UNKNOWN:
		break;
	case STANDARD_HOST:
		charger_type = POWER_SUPPLY_TYPE_USB;
		break;
	case CHARGING_HOST:
		charger_type = POWER_SUPPLY_TYPE_USB_CDP;
		break;
	case NONSTANDARD_CHARGER:
	case APPLE_0_5A_CHARGER:
	case STANDARD_CHARGER:
	case APPLE_2_1A_CHARGER:
	case APPLE_1_0A_CHARGER:
		charger_type = POWER_SUPPLY_TYPE_USB_DCP;
		break;
	default:
		break;
	}
	chg_debug("mt_power_supply_type_check-----2---------charger_type = %d,charger_type_final = %d,g_chr_type=%d\r\n",charger_type,charger_type_final,g_chr_type);
	return charger_type;

}
#endif /*ODM_WT_EDIT*/
#ifndef ODM_WT_EDIT
/*Junbo.Guo@ODM_WT.BSP.CHG 2019/11/13, add for bulid err*/
enum {
    Channel_12 = 2,
    Channel_13,
    Channel_14,
};
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);
extern int IMM_IsAdcInitReady(void);
int mt_vadc_read(int times, int Channel)
{
    int ret = 0, data[4], i, ret_value = 0, ret_temp = 0;
    if( IMM_IsAdcInitReady() == 0 )
    {
        return 0;
    }
    i = times ;
    while (i--)
    {
	ret_value = IMM_GetOneChannelValue(Channel, data, &ret_temp);
	if(ret_value != 0)
	{
		i++;
		continue;
	}
	ret += ret_temp;
    }
	ret = ret*1500/4096;
    ret = ret/times;
	//chg_debug("[mt_vadc_read] Channel %d: vol_ret=%d\n",Channel,ret);
	return ret;
}
#endif
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/12, add for usbtemp*/
int usbtemp_channel_init(struct device *dev)
{
	int ret = 0;
	usb_chan1 = iio_channel_get(dev, "usbtemp-ch3");
	if(IS_ERR_OR_NULL(usb_chan1)){
		chg_err("usb_chan1 init fial,err = %d",PTR_ERR(usb_chan1));
		ret = -1;
	}

	usb_chan2 = iio_channel_get(dev, "usbtemp-ch4");
	if(IS_ERR_OR_NULL(usb_chan2)){
		chg_err("usb_chan2 init fial,err = %d",PTR_ERR(usb_chan2));
		ret = -1;
	}

	return ret;
}
#endif /*ODM_WT_EDIT*/

static void set_usbswitch_to_rxtx(struct oppo_chg_chip *chip)
{
	int ret = 0;
	gpio_direction_output(chip->normalchg_gpio.chargerid_switch_gpio, 1);
	ret = pinctrl_select_state(chip->normalchg_gpio.pinctrl, chip->normalchg_gpio.charger_gpio_as_output2);
	if (ret < 0) {
		chg_err("failed to set pinctrl int\n");
		return ;
	}
	chg_err("set_usbswitch_to_rxtx \n");
}
static void set_usbswitch_to_dpdm(struct oppo_chg_chip *chip)
{
	int ret = 0;
	gpio_direction_output(chip->normalchg_gpio.chargerid_switch_gpio, 0);
	ret = pinctrl_select_state(chip->normalchg_gpio.pinctrl, chip->normalchg_gpio.charger_gpio_as_output1);
	if (ret < 0) {
		chg_err("failed to set pinctrl int\n");
		return ;
	}
	chg_err("set_usbswitch_to_dpdm \n");
}
static bool is_support_chargerid_check(void)
{

#ifdef CONFIG_OPPO_CHECK_CHARGERID_VOLT
	return true;
#else
	return false;
#endif

}
int mt_get_chargerid_volt (void)
{
	int chargerid_volt = 0;
	if(is_support_chargerid_check() == true)
	{
#ifndef ODM_WT_EDIT
/*Junbo.Guo@ODM_WT.BSP.CHG 2019/11/13, add for bulid err*/
		chargerid_volt = mt_vadc_read(10,Channel_14);//get the charger id volt
#endif
		chg_debug("chargerid_volt = %d \n",
					   chargerid_volt);
	}
		else
		{
		chg_debug("is_support_chargerid_check = false !\n");
		return 0;
	}
	return chargerid_volt;
		}


void mt_set_chargerid_switch_val(int value)
{
	chg_debug("set_value= %d\n",value);
	if(NULL == g_oppo_chip)
		return;
	if(is_support_chargerid_check() == false)
		return;
	if(g_oppo_chip->normalchg_gpio.chargerid_switch_gpio <= 0) {
		chg_err("chargerid_switch_gpio not exist, return\n");
		return;
	}
	if(IS_ERR_OR_NULL(g_oppo_chip->normalchg_gpio.pinctrl)
		|| IS_ERR_OR_NULL(g_oppo_chip->normalchg_gpio.charger_gpio_as_output1)
		|| IS_ERR_OR_NULL(g_oppo_chip->normalchg_gpio.charger_gpio_as_output2)) {
		chg_err("pinctrl null, return\n");
		return;
	}
	if(1 == value){
			set_usbswitch_to_rxtx(g_oppo_chip);
	}else if(0 == value){
		set_usbswitch_to_dpdm(g_oppo_chip);
	}else{
		//do nothing
	}
	chg_debug("get_val:%d\n",gpio_get_value(g_oppo_chip->normalchg_gpio.chargerid_switch_gpio));
}

int mt_get_chargerid_switch_val(void)
{
	int gpio_status = 0;
	if(NULL == g_oppo_chip)
		return 0;
	if(is_support_chargerid_check() == false)
		return 0;
	gpio_status = gpio_get_value(g_oppo_chip->normalchg_gpio.chargerid_switch_gpio);

	chg_debug("mt_get_chargerid_switch_val:%d\n",gpio_status);
	return gpio_status;
}


int oppo_usb_switch_gpio_gpio_init(void)
{
	chg_err("---1-----");
	g_oppo_chip->normalchg_gpio.pinctrl = devm_pinctrl_get(g_oppo_chip->dev);
    if (IS_ERR_OR_NULL(g_oppo_chip->normalchg_gpio.pinctrl)) {
       chg_err("get normalchg_gpio.chargerid_switch_gpio pinctrl falil\n");
		return -EINVAL;
    }
    g_oppo_chip->normalchg_gpio.charger_gpio_as_output1 = pinctrl_lookup_state(g_oppo_chip->normalchg_gpio.pinctrl,
								"charger_gpio_as_output_low");
    if (IS_ERR_OR_NULL(g_oppo_chip->normalchg_gpio.charger_gpio_as_output1)) {
       	chg_err("get charger_gpio_as_output_low fail\n");
			return -EINVAL;
    }
	g_oppo_chip->normalchg_gpio.charger_gpio_as_output2 = pinctrl_lookup_state(g_oppo_chip->normalchg_gpio.pinctrl,
								"charger_gpio_as_output_high");
	if (IS_ERR_OR_NULL(g_oppo_chip->normalchg_gpio.charger_gpio_as_output2)) {
		chg_err("get charger_gpio_as_output_high fail\n");
		return -EINVAL;
	}

	pinctrl_select_state(g_oppo_chip->normalchg_gpio.pinctrl,g_oppo_chip->normalchg_gpio.charger_gpio_as_output1);
	return 0;
}


int charger_pretype_get(void)
{
	int chg_type = STANDARD_HOST;
	//chg_type = hw_charging_get_charger_type();
	return chg_type;
}


bool oppo_pmic_check_chip_is_null(void)
{
	if (!is_vooc_project()) {
		return true;
	} else {
		return false;
	}
}

//====================================================================//
#ifdef VENDOR_EDIT
/* Jianchao.Shi@BSP.CHG.Basic, 2019/07/25, sjc Add for usbtemp */
static bool oppo_usbtemp_check_is_gpio(struct oppo_chg_chip *chip)
{
	if (!chip) {
		printk(KERN_ERR "[OPPO_CHG][%s]: oppo_chip not ready!\n", __func__);
		return false;
	}

	if (gpio_is_valid(chip->normalchg_gpio.dischg_gpio))
		return true;

	return false;
}

static bool oppo_usbtemp_check_is_support(void)
{
	struct oppo_chg_chip *chip = g_oppo_chip;

	if (!chip) {
		printk(KERN_ERR "[OPPO_CHG][%s]: oppo_chip not ready!\n", __func__);
		return false;
	}

	if (oppo_usbtemp_check_is_gpio(chip) == true)
		return true;

	chg_err("not support, return false\n");

	return false;
}

static int oppo_dischg_gpio_init(struct oppo_chg_chip *chip)
{
	if (!chip) {
		chg_err("oppo_chip not ready!\n");
		return -EINVAL;
	}

	chip->normalchg_gpio.pinctrl = devm_pinctrl_get(chip->dev);

	if (IS_ERR_OR_NULL(chip->normalchg_gpio.pinctrl)) {
		chg_err("get dischg_pinctrl fail\n");
		return -EINVAL;
	}

	chip->normalchg_gpio.dischg_enable = pinctrl_lookup_state(chip->normalchg_gpio.pinctrl, "dischg_enable");
	if (IS_ERR_OR_NULL(chip->normalchg_gpio.dischg_enable)) {
		chg_err("get dischg_enable fail\n");
		return -EINVAL;
	}

	chip->normalchg_gpio.dischg_disable = pinctrl_lookup_state(chip->normalchg_gpio.pinctrl, "dischg_disable");
	if (IS_ERR_OR_NULL(chip->normalchg_gpio.dischg_disable)) {
		chg_err("get dischg_disable fail\n");
		return -EINVAL;
	}

	pinctrl_select_state(chip->normalchg_gpio.pinctrl, chip->normalchg_gpio.dischg_disable);

	return 0;
}


#define USB_20C		20//degreeC
#define USB_50C		50
#define USB_57C		57
#define USB_100C	100
#define USB_25C_VOLT	1192//900//mv
#define USB_50C_VOLT	450
#define USB_55C_VOLT	448
#define USB_60C_VOLT	327
#define VBUS_VOLT_THRESHOLD	3000//3V
#define RETRY_CNT_DELAY		5 //ms
#define MIN_MONITOR_INTERVAL	50//50ms
#define VBUS_MONITOR_INTERVAL	3000//3s

#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/12, add for usbtemp*/
#define USB_PORT_PULL_UP_R      390000 //390K
#define USB_PORT_PULL_UP_VOLT   1800  //1.8V
#define USB_NTC_TABLE_SIZE 74

struct USB_PORT_TEMPERATURE {
	signed int Temp;
	signed int TemperatureR;
};

struct USB_PORT_TEMPERATURE Usb_Port_Temperature_Table[USB_NTC_TABLE_SIZE] = {
/* NCP15WF104F03RC(100K) */
	{-40, 4397119},
	{-35, 3088599},
	{-30, 2197225},
	{-25, 1581881},
	{-20, 1151037},
	{-15, 846579},
	{-10, 628988},
	{-5, 471632},
	{0, 357012},
	{5, 272500},
	{10, 209710},
	{15, 162651},
	{20, 127080},
	{25, 100000},		/* 100K */
	{30, 79222},
	{35, 63167},
	{40, 50677},
	{41, 48528},
	{42, 46482},
	{43, 44533},
	{44, 42675},
	{45, 40904},
	{46, 39213},
	{47, 37601},
	{48, 36063},
	{49, 34595},
	{50, 33195},
	{51, 31859},
	{52, 30584},
	{53, 29366},
	{54, 28203},
	{55, 27091},
	{56, 26028},
	{57, 25013},
	{58, 24042},
	{59, 23113},
	{60, 22224},
	{61, 21374},
	{62, 20560},
	{63, 19782},
	{64, 19036},
	{65, 18322},
	{66, 17640},
	{67, 16986},
	{68, 16360},
	{69, 15759},
	{70, 15184},
	{71, 14631},
	{72, 14100},
	{73, 13591},
	{74, 13103},
	{75, 12635},
	{76, 12187},
	{77, 11756},
	{78, 11343},
	{79, 10946},
	{80, 10565},
	{81, 10199},
	{82,  9847},
	{83,  9509},
	{84,  9184},
	{85,  8872},
	{86,  8572},
	{87,  8283},
	{88,  8005},
	{89,  7738},
	{90,  7481},
	{95,  6337},
	{100, 5384},
	{105, 4594},
	{110, 3934},
	{115, 3380},
	{120, 2916},
	{125, 2522}
};
#endif /*ODM_WT_EDIT*/

static void oppo_get_usbtemp_volt(struct oppo_chg_chip *chip)
{
	int usbtemp_volt = 0;

	if (!chip) {
		printk(KERN_ERR "[OPPO_CHG][%s]: oppo_chip not ready!\n", __func__);
		return;
	}
#ifndef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/12, add for usbtemp*/
	usbtemp_volt = mt_vadc_read(1, Channel_13);//get the typec tem adc volt
	if (usbtemp_volt <= 0) {
		usbtemp_volt = USB_25C_VOLT;
	}
	chip->usbtemp_volt_r = usbtemp_volt;

	usbtemp_volt = mt_vadc_read(1, Channel_12);//get the typec tem adc2 volt
	if (usbtemp_volt <= 0) {
		usbtemp_volt = USB_25C_VOLT;
	}
	chip->usbtemp_volt_l = usbtemp_volt;
#else
	iio_read_channel_processed(usb_chan1, &usbtemp_volt);
	if (usbtemp_volt <= 0) {
		usbtemp_volt = USB_25C_VOLT;
	}
	usbtemp_volt = (usbtemp_volt * 1500) >> 12;
	chip->usbtemp_volt_r = usbtemp_volt;
	
	iio_read_channel_processed(usb_chan2, &usbtemp_volt);
	if (usbtemp_volt <= 0) {
		usbtemp_volt = USB_25C_VOLT;
	}
	usbtemp_volt = (usbtemp_volt * 1500) >> 12;
	chip->usbtemp_volt_l = usbtemp_volt;
#endif /*ODM_WT_EDIT*/

	//chg_err("usbtemp_volt: %d, %d\n", chip->usbtemp_volt_r, chip->usbtemp_volt_l);

	return;
}
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/12, add for usbtemp*/
int usb_port_volt_to_temp(int volt)
{
	int i = 0;
	int usb_r = 0;
	int RES1 = 0, RES2 = 0;
	int Usb_temp_Value = 25, TMP1 = 0, TMP2 = 0;
	
	if(usb_debug_temp != 65535)
		return usb_debug_temp;

	usb_r = volt * USB_PORT_PULL_UP_R / (USB_PORT_PULL_UP_VOLT - volt);
	pr_debug("[UsbTemp] NTC_R = %d\n",usb_r);

	if (usb_r >= Usb_Port_Temperature_Table[0].TemperatureR) {
		Usb_temp_Value = -40;
	} else if (usb_r <= Usb_Port_Temperature_Table[USB_NTC_TABLE_SIZE - 1].TemperatureR) {
		Usb_temp_Value = 125;
	} else {
		RES1 = Usb_Port_Temperature_Table[0].TemperatureR;
		TMP1 = Usb_Port_Temperature_Table[0].Temp;

		for (i = 0; i < USB_NTC_TABLE_SIZE; i++) {
			if (usb_r >= Usb_Port_Temperature_Table[i].TemperatureR) {
				RES2 = Usb_Port_Temperature_Table[i].TemperatureR;
				TMP2 = Usb_Port_Temperature_Table[i].Temp;
				break;
			}
			{	/* hidden else */
				RES1 = Usb_Port_Temperature_Table[i].TemperatureR;
				TMP1 = Usb_Port_Temperature_Table[i].Temp;
			}
		}

		Usb_temp_Value = (((usb_r - RES2) * TMP1) +
			((RES1 - usb_r) * TMP2)) / (RES1 - RES2);
	}
	pr_debug(
		"[%s] %d %d %d %d %d %d\n",
		__func__,
		RES1, RES2, usb_r, TMP1,
		TMP2, Usb_temp_Value);

	return Usb_temp_Value;
}
#endif /*ODM_WT_EDIT*/

static void get_usb_temp(struct oppo_chg_chip *chip)
{
#ifndef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/12, add for usbtemp*/
	int i = 0;

	if (!chip) {
		printk(KERN_ERR "[OPPO_CHG][%s]: oppo_chip not ready!\n", __func__);
		return;
	}

	for (i = ARRAY_SIZE(con_volt_18097) - 1; i >= 0; i--) {
		if (con_volt_18097[i] >= chip->usbtemp_volt_r)
			break;
		else if (i == 0)
			break;
	}
	chip->usb_temp_r = con_temp_18097[i];

	for (i = ARRAY_SIZE(con_volt_18097) - 1; i >= 0; i--) {
		if (con_volt_18097[i] >= chip->usbtemp_volt_l)
			break;
		else if (i == 0)
			break;
	}
	chip->usb_temp_l = con_temp_18097[i];
#else /*ODM_WT_EDIT*/
	if (!chip) {
		printk(KERN_ERR "[OPPO_CHG][%s]: oppo_chip not ready!\n", __func__);
		return;
	}

	chip->usb_temp_r = usb_port_volt_to_temp(chip->usbtemp_volt_r);
	chip->usb_temp_l = usb_port_volt_to_temp(chip->usbtemp_volt_l);
#endif /*ODM_WT_EDIT*/

	pr_debug("usbtemp: %d, %d\n", chip->usb_temp_r, chip->usb_temp_l);

	return;
}

#ifdef ODM_WT_EDIT
static bool oppo_chg_get_vbus_status(struct oppo_chg_chip *chip)
{
    int charger_type;
	if (!chip) {
		printk(KERN_ERR "[OPPO_CHG][%s]: oppo_chip not ready!\n", __func__);
		return false;
	}
	
	charger_type = chip->chg_ops->get_charger_type();
	if(charger_type)
		return true;
	else
		return false;
}
#else
static bool oppo_chg_get_vbus_status(struct oppo_chg_chip *chip)
{
	if (!chip) {
		printk(KERN_ERR "[OPPO_CHG][%s]: oppo_chip not ready!\n", __func__);
		return false;
	}

	return chip->charger_exist;
}
#endif

#define RETRY_COUNT		3
static int oppo_usbtemp_monitor_main(void *data)
{
	int i = 0;
	int delay = 0;
	int vbus_volt = 0;
	int count_r = 0;
	int count_l = 0;
	int time_count = 0;
	static bool dischg_flag = false;
	static int count = 0;
	static int last_usb_temp_r = 25;
	static int last_usb_temp_l = 25;
	static bool init_flag = true;
	static int retry_usb_temp_r = 25;
	static int retry_usb_temp_l = 25;
	struct oppo_chg_chip *chip = g_oppo_chip;

	while (!kthread_should_stop() && dischg_flag == false) {
		oppo_get_usbtemp_volt(chip);
		get_usb_temp(chip);

		if (init_flag == true) {
			init_flag = false;
			if (oppo_chg_get_vbus_status(chip) == false
					&& oppo_chg_get_otg_online() == false) {
				delay = MIN_MONITOR_INTERVAL * 20;
				goto check_again;
			}
		}
		if (chip->usb_temp_r < USB_50C && chip->usb_temp_l < USB_50C)//get vbus when usbtemp < 50C
			vbus_volt = battery_meter_get_charger_voltage();
		else
			vbus_volt = 0;

		delay = MIN_MONITOR_INTERVAL;
		time_count = 30;

		if (chip->usb_temp_r < USB_50C && chip->usb_temp_l < USB_50C && vbus_volt < VBUS_VOLT_THRESHOLD)
			delay = VBUS_MONITOR_INTERVAL;

		if ((chip->usb_temp_r >= USB_57C)
				|| (chip->usb_temp_l >= USB_57C)) {
			for (i = 0; i < RETRY_COUNT; i++) {
				mdelay(RETRY_CNT_DELAY);
				oppo_get_usbtemp_volt(chip);
				get_usb_temp(chip);
#ifdef ODM_WT_EDIT
/*Junbo.Guo@ODM_WT.BSP.CHG 2020/1/7, add for usbtemp*/
				if ((chip->usb_temp_r >= USB_57C)&&(abs(retry_usb_temp_r-chip->usb_temp_r)<5))
					count_r++;
				if ((chip->usb_temp_l >= USB_57C)&&(abs(retry_usb_temp_l-chip->usb_temp_l)<5))
					count_l++;
				
				retry_usb_temp_r = chip->usb_temp_r;
				retry_usb_temp_l = chip->usb_temp_l;
				pr_debug("high temp: %d,%d,%d,%d,%d,%d\n", chip->usb_temp_r, chip->usb_temp_l,retry_usb_temp_r,retry_usb_temp_l,count_r,count_l);
#else
				if(chip->usb_temp_r >= USB_57C)
					count_r++;
				if(chip->usb_temp_l >= USB_57C)
					count_l++;
#endif
			}

			if (count_r >= RETRY_COUNT || count_l >= RETRY_COUNT) {
				if (!IS_ERR_OR_NULL(chip->normalchg_gpio.dischg_enable)) {
					dischg_flag = true;
					chg_err("dischg enable...usb_temp[%d,%d], usb_volt[%d,%d]\n",
							chip->usb_temp_r, chip->usb_temp_l, chip->usbtemp_volt_r, chip->usbtemp_volt_l);
					oppo_set_usb_status(USB_TEMP_HIGH);
					if (oppo_chg_get_otg_online() == true) {
						oppo_set_otg_switch_status(false);
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/12/23, wait for vbus fall to make sure otg disable*/
						for(i = 0; i < 100 ; i++){
							if(battery_get_vbus() < 2500)
								break;
							usleep_range(10000, 11000);
						}
#endif
					}
					if (oppo_vooc_get_fastchg_started() == true) {
						oppo_chg_set_chargerid_switch_val(0);
						oppo_vooc_switch_mode(NORMAL_CHARGER_MODE);
						oppo_vooc_reset_mcu();
						//msleep(20);//wait for turn-off fastchg MOS
					}
					chip->chg_ops->charging_disable();
					if(chip->chg_ops->oppo_chg_set_hz_mode != NULL)
						chip->chg_ops->oppo_chg_set_hz_mode(true);
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/12/23, disable sub charger*/
					if(chip->is_double_charger_support)
						chip->sub_chg_ops->charging_disable();
#endif
					usleep_range(10000, 11000);
					chip->chg_ops->charger_suspend();
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/12/23, disable sub charger*/
					if(chip->is_double_charger_support)
						chip->sub_chg_ops->charger_suspend();
#endif
					usleep_range(10000, 11000);
					pinctrl_select_state(chip->normalchg_gpio.pinctrl, chip->normalchg_gpio.dischg_enable);
				}
			}

			count_r = 0;
			count_l = 0;
			count = 0;
		} else if (((chip->usb_temp_r - chip->temperature/10) > 10)
				|| ((chip->usb_temp_l - chip->temperature/10) > 10)) {
			if (count <= time_count) {
				if (count == 0) {
					last_usb_temp_r = chip->usb_temp_r;
					last_usb_temp_l = chip->usb_temp_l;
				}

				if (((chip->usb_temp_r - last_usb_temp_r) >= 3 && chip->usb_temp_r >= USB_20C)
						|| ((chip->usb_temp_l - last_usb_temp_l) >= 3 && chip->usb_temp_l >= USB_20C)) {
					for (i = 0; i < RETRY_COUNT; i++) {
						mdelay(RETRY_CNT_DELAY);
						oppo_get_usbtemp_volt(chip);
						get_usb_temp(chip);
						if ((chip->usb_temp_r - last_usb_temp_r) >= 3)
							count_r++;
						if ((chip->usb_temp_l - last_usb_temp_l) >= 3)
							count_l++;
					}

					if (count_r >= RETRY_COUNT || count_l >= RETRY_COUNT) {
						if (!IS_ERR_OR_NULL(chip->normalchg_gpio.dischg_enable)) {
							dischg_flag = true;
							chg_err("dischg enable...current_usb_temp[%d,%d], last_usb_temp[%d,%d], count[%d]\n",
									chip->usb_temp_r, chip->usb_temp_l, last_usb_temp_r, last_usb_temp_l, count);
							oppo_set_usb_status(USB_TEMP_HIGH);
							if (oppo_chg_get_otg_online() == true) {
								oppo_set_otg_switch_status(false);
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/12/23, wait for vbus fall to make sure otg disable*/
								for(i = 0; i < 100 ; i++){
									if(battery_get_vbus() < 2500)
										break;
									usleep_range(10000, 11000);
								}
#endif
							}
							if (oppo_vooc_get_fastchg_started() == true) {
								oppo_chg_set_chargerid_switch_val(0);
								oppo_vooc_switch_mode(NORMAL_CHARGER_MODE);
								oppo_vooc_reset_mcu();
								//msleep(20);//wait for turn-off fastchg MOS
							}
							chip->chg_ops->charging_disable();
							if(chip->chg_ops->oppo_chg_set_hz_mode != NULL)
								chip->chg_ops->oppo_chg_set_hz_mode(true);
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/12/23, disable sub charger*/
							if(chip->is_double_charger_support)
								chip->sub_chg_ops->charging_disable();
#endif
							usleep_range(10000, 11000);
							chip->chg_ops->charger_suspend();
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/12/23, disable sub charger*/
							if(chip->is_double_charger_support)
								chip->sub_chg_ops->charger_suspend();
#endif
							usleep_range(10000, 11000);
							pinctrl_select_state(chip->normalchg_gpio.pinctrl, chip->normalchg_gpio.dischg_enable);
						}
					}
					count_r = 0;
					count_l = 0;
				}

				count++;
				if (count > time_count) {
					count = 0;
				}
			}
			msleep(delay);
		} else {
check_again:
			count = 0;
			last_usb_temp_r = chip->usb_temp_r;
			last_usb_temp_l = chip->usb_temp_l;
			msleep(delay);
			wait_event_interruptible(oppo_usbtemp_wq,
				oppo_chg_get_vbus_status(chip) == true || oppo_chg_get_otg_online() == true);
		}
	}

	return 0;
}

static void oppo_usbtemp_thread_init(void)
{
	oppo_usbtemp_kthread =
			kthread_run(oppo_usbtemp_monitor_main, 0, "usbtemp_kthread");
	if (IS_ERR(oppo_usbtemp_kthread)) {
		chg_err("failed to cread oppo_usbtemp_kthread\n");
	}
}

void oppo_wake_up_usbtemp_thread(void)
{
	if (oppo_usbtemp_check_is_support() == true) {
		wake_up_interruptible(&oppo_usbtemp_wq);
		chg_debug("wake_up_usbtemp_thread, vbus:%d, otg:%d",oppo_chg_get_vbus_status(g_oppo_chip),oppo_chg_get_otg_online());
	}
}
EXPORT_SYMBOL(oppo_wake_up_usbtemp_thread);

static int oppo_chg_usbtemp_parse_dt(struct oppo_chg_chip *chip)
{
	int rc = 0;
	struct device_node *node = NULL;

	if (chip)
		node = chip->dev->of_node;
	if (node == NULL) {
		chg_err("oppo_chip or device tree info. missing\n");
		return -EINVAL;
	}

	chip->normalchg_gpio.dischg_gpio = of_get_named_gpio(node, "qcom,dischg-gpio", 0);
	if (chip->normalchg_gpio.dischg_gpio <= 0) {
		chg_err("Couldn't read qcom,dischg-gpio rc=%d, qcom,dischg-gpio:%d\n",
				rc, chip->normalchg_gpio.dischg_gpio);
	} else {
		if (oppo_usbtemp_check_is_support() == true) {
			rc = gpio_request(chip->normalchg_gpio.dischg_gpio, "dischg-gpio");
			if (rc) {
				chg_err("unable to request dischg-gpio:%d\n",
						chip->normalchg_gpio.dischg_gpio);
			} else {
				rc = oppo_dischg_gpio_init(chip);
				if (rc)
					chg_err("unable to init dischg-gpio:%d\n",
							chip->normalchg_gpio.dischg_gpio);
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/12, add for usbtemp*/
				rc = usbtemp_channel_init(chip->dev);
				if (rc)
					chg_err("unable to init usbtemp_channel\n");
#endif	/*ODM_WT_EDIT*/
			}
		}
		chg_err("dischg-gpio:%d\n", chip->normalchg_gpio.dischg_gpio);
	}

	return rc;
}
#endif /* VENDOR_EDIT */
//====================================================================//

#endif


/************************************************/
/* Power Supply Functions
*************************************************/
static int mt_ac_get_property(struct power_supply *psy,
	enum power_supply_property psp, union power_supply_propval *val)
{
    int rc = 0;
    rc = oppo_ac_get_property(psy, psp, val);

	return 0;
}

static int mt_usb_prop_is_writeable(struct power_supply *psy,
		enum power_supply_property psp)
{
	int rc = 0;
	rc = oppo_usb_property_is_writeable(psy, psp);
	return rc;
}

static int mt_usb_get_property(struct power_supply *psy,
	enum power_supply_property psp, union power_supply_propval *val)
{
	int rc = 0;

	switch (psp) {
#if defined CONFIG_OPPO_CHARGER_MT6370_TYPEC || defined ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/11, add for typec_cc_orientation node*/
		case POWER_SUPPLY_PROP_TYPEC_CC_ORIENTATION:
			val->intval = oppo_get_typec_cc_orientation();
			break;
#endif
		case POWER_SUPPLY_PROP_USB_STATUS:
			val->intval = oppo_get_usb_status();
			break;
		case POWER_SUPPLY_PROP_USBTEMP_VOLT_L:
			if (g_oppo_chip)
				val->intval = g_oppo_chip->usbtemp_volt_l;
			else
				val->intval = -ENODATA;
			break;
		case POWER_SUPPLY_PROP_USBTEMP_VOLT_R:
			if (g_oppo_chip)
				val->intval = g_oppo_chip->usbtemp_volt_r;
			else
				val->intval = -ENODATA;
			break;
#ifdef ODM_WT_EDIT
//Junbo.Guo@ODM_WT.BSP.CHG, 2019/11/11, Modify for fastcharger mode
		case POWER_SUPPLY_PROP_FAST_CHG_TYPE:
		if (!g_oppo_chip->chg_ops->get_charger_subtype){
			 val->intval = 0;
		 }else{
			val->intval=g_oppo_chip->chg_ops->get_charger_subtype();
		}
		break;
#endif
		default:
			rc = oppo_usb_get_property(psy, psp, val);
	}

	return rc;
}

static int mt_usb_set_property(struct power_supply *psy,
	enum power_supply_property psp, const union power_supply_propval *val)
{
	oppo_usb_set_property(psy, psp, val);
	return 0;
}

static int battery_prop_is_writeable(struct power_supply *psy,
		enum power_supply_property psp)
{
	int rc = 0;
	rc = oppo_battery_property_is_writeable(psy, psp);
	return rc;
}

static int battery_set_property(struct power_supply *psy,
	enum power_supply_property psp, const union power_supply_propval *val)
{
	oppo_battery_set_property(psy, psp, val);
	return 0;
}

static int battery_get_property(struct power_supply *psy,
	enum power_supply_property psp, union power_supply_propval *val)
{
	oppo_battery_get_property(psy, psp, val);
	return 0;
}


static enum power_supply_property mt_ac_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
#ifdef CONFIG_OPPO_FAST2NORMAL_CHG
	POWER_SUPPLY_PROP_FAST2NORMAL_CHG,
#endif
};

static enum power_supply_property mt_usb_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_VOLTAGE_MAX,
	POWER_SUPPLY_PROP_OTG_SWITCH,
	POWER_SUPPLY_PROP_OTG_ONLINE,
#if defined CONFIG_OPPO_CHARGER_MT6370_TYPEC || defined ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/11, add for typec_cc_orientation node*/
	POWER_SUPPLY_PROP_TYPEC_CC_ORIENTATION,
#endif
	POWER_SUPPLY_PROP_USB_STATUS,
	POWER_SUPPLY_PROP_USBTEMP_VOLT_L,
	POWER_SUPPLY_PROP_USBTEMP_VOLT_R,
#ifdef ODM_WT_EDIT
//Junbo.Guo@ODM_WT.BSP.CHG, 2019/11/11, Modify for fastcharger mode
	POWER_SUPPLY_PROP_FAST_CHG_TYPE,
#endif
};
static enum power_supply_property battery_properties[] = {
        POWER_SUPPLY_PROP_STATUS,
        POWER_SUPPLY_PROP_HEALTH,
        POWER_SUPPLY_PROP_PRESENT,
        POWER_SUPPLY_PROP_TECHNOLOGY,
        POWER_SUPPLY_PROP_CAPACITY,
        POWER_SUPPLY_PROP_TEMP,
        POWER_SUPPLY_PROP_VOLTAGE_NOW,
        POWER_SUPPLY_PROP_VOLTAGE_MIN,
        POWER_SUPPLY_PROP_CURRENT_NOW,
        POWER_SUPPLY_PROP_CHARGE_NOW,
        POWER_SUPPLY_PROP_AUTHENTICATE,
        POWER_SUPPLY_PROP_CHARGE_TIMEOUT,
        POWER_SUPPLY_PROP_CHARGE_TECHNOLOGY,
        POWER_SUPPLY_PROP_FAST_CHARGE,
        POWER_SUPPLY_PROP_MMI_CHARGING_ENABLE,        /*add for MMI_CHG_TEST*/
#ifdef CONFIG_OPPO_CHARGER_MTK
        POWER_SUPPLY_PROP_STOP_CHARGING_ENABLE,
        POWER_SUPPLY_PROP_CHARGE_FULL,
        POWER_SUPPLY_PROP_CHARGE_COUNTER,
        POWER_SUPPLY_PROP_CURRENT_MAX,
#endif
        POWER_SUPPLY_PROP_BATTERY_FCC,
        POWER_SUPPLY_PROP_BATTERY_SOH,
        POWER_SUPPLY_PROP_BATTERY_CC,
        POWER_SUPPLY_PROP_BATTERY_RM,
        POWER_SUPPLY_PROP_BATTERY_NOTIFY_CODE,

#ifdef VENDOR_EDIT
/* Yichun.Chen  PSW.BSP.CHG  2019-05-14  for soc node */
	POWER_SUPPLY_PROP_CHIP_SOC,
	POWER_SUPPLY_PROP_SMOOTH_SOC,
#endif

        POWER_SUPPLY_PROP_ADAPTER_FW_UPDATE,
        POWER_SUPPLY_PROP_VOOCCHG_ING,
#ifdef CONFIG_OPPO_CHECK_CHARGERID_VOLT
        POWER_SUPPLY_PROP_CHARGERID_VOLT,
#endif
#ifdef CONFIG_OPPO_SHIP_MODE_SUPPORT
        POWER_SUPPLY_PROP_SHIP_MODE,
#endif
#ifdef CONFIG_OPPO_CALL_MODE_SUPPORT
        POWER_SUPPLY_PROP_CALL_MODE,
#endif
#ifdef CONFIG_OPPO_SHORT_C_BATT_CHECK
#ifdef CONFIG_OPPO_SHORT_USERSPACE
        POWER_SUPPLY_PROP_SHORT_C_LIMIT_CHG,
        POWER_SUPPLY_PROP_SHORT_C_LIMIT_RECHG,
        POWER_SUPPLY_PROP_CHARGE_TERM_CURRENT,
        POWER_SUPPLY_PROP_INPUT_CURRENT_SETTLED,
#else
        POWER_SUPPLY_PROP_SHORT_C_BATT_UPDATE_CHANGE,
        POWER_SUPPLY_PROP_SHORT_C_BATT_IN_IDLE,
        POWER_SUPPLY_PROP_SHORT_C_BATT_CV_STATUS,
#endif /*CONFIG_OPPO_SHORT_USERSPACE*/
#endif
#ifdef CONFIG_OPPO_SHORT_HW_CHECK
        POWER_SUPPLY_PROP_SHORT_C_HW_FEATURE,
        POWER_SUPPLY_PROP_SHORT_C_HW_STATUS,
#endif
#ifdef CONFIG_OPPO_SHORT_IC_CHECK
        POWER_SUPPLY_PROP_SHORT_C_IC_OTP_STATUS,
        POWER_SUPPLY_PROP_SHORT_C_IC_VOLT_THRESH,
        POWER_SUPPLY_PROP_SHORT_C_IC_OTP_VALUE,
#endif
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/11/25, add for battery info node*/
	POWER_SUPPLY_PROP_BAT_ID_VOLT,
	POWER_SUPPLY_PROP_BAT_TYPE,
#endif
};


static int oppo_power_supply_init(struct oppo_chg_chip *chip)
{
    int ret = 0;
    struct oppo_chg_chip *mt_chg = NULL;
    mt_chg = chip;
    mt_chg->ac_psd.name = "ac";
    mt_chg->ac_psd.type = POWER_SUPPLY_TYPE_MAINS;
    mt_chg->ac_psd.properties = mt_ac_properties;
    mt_chg->ac_psd.num_properties = ARRAY_SIZE(mt_ac_properties);
    mt_chg->ac_psd.get_property = mt_ac_get_property;
    mt_chg->ac_cfg.drv_data = mt_chg;

    mt_chg->usb_psd.name = "usb";
    mt_chg->usb_psd.type = POWER_SUPPLY_TYPE_USB;
    mt_chg->usb_psd.properties = mt_usb_properties;
    mt_chg->usb_psd.num_properties = ARRAY_SIZE(mt_usb_properties);
    mt_chg->usb_psd.get_property = mt_usb_get_property;
    mt_chg->usb_psd.set_property = mt_usb_set_property;
    mt_chg->usb_psd.property_is_writeable = mt_usb_prop_is_writeable;
    mt_chg->usb_cfg.drv_data = mt_chg;

    mt_chg->battery_psd.name = "battery";
    mt_chg->battery_psd.type = POWER_SUPPLY_TYPE_BATTERY;
    mt_chg->battery_psd.properties = battery_properties;
    mt_chg->battery_psd.num_properties = ARRAY_SIZE(battery_properties);
    mt_chg->battery_psd.get_property = battery_get_property;
    mt_chg->battery_psd.set_property = battery_set_property;
    mt_chg->battery_psd.property_is_writeable = battery_prop_is_writeable,


    mt_chg->ac_psy = power_supply_register(mt_chg->dev, &mt_chg->ac_psd,
        &mt_chg->ac_cfg);
    if (IS_ERR(mt_chg->ac_psy)) {
        dev_err(mt_chg->dev, "Failed to register power supply: %ld\n",
            PTR_ERR(mt_chg->ac_psy));
        ret = PTR_ERR(mt_chg->ac_psy);
        goto err_ac_psy;
    }
    mt_chg->usb_psy = power_supply_register(mt_chg->dev, &mt_chg->usb_psd,
        &mt_chg->usb_cfg);
    if (IS_ERR(mt_chg->usb_psy)) {
        dev_err(mt_chg->dev, "Failed to register power supply: %ld\n",
            PTR_ERR(mt_chg->usb_psy));
        ret = PTR_ERR(mt_chg->usb_psy);
        goto err_usb_psy;
    }
    mt_chg->batt_psy = power_supply_register(mt_chg->dev, &mt_chg->battery_psd,
        NULL);
    if (IS_ERR(mt_chg->batt_psy)) {
        dev_err(mt_chg->dev, "Failed to register power supply: %ld\n",
            PTR_ERR(mt_chg->batt_psy));
        ret = PTR_ERR(mt_chg->batt_psy);
        goto err_battery_psy;
    }
    pr_info("%s\n", __func__);
    return 0;

err_usb_psy:
    power_supply_unregister(mt_chg->ac_psy);
err_ac_psy:
    power_supply_unregister(mt_chg->usb_psy);
err_battery_psy:
    power_supply_unregister(mt_chg->batt_psy);

    return ret;
}


static int oppo_chg_parse_charger_dt_2nd_override(struct oppo_chg_chip *chip)
{
	int rc;
	struct device_node *node = chip->dev->of_node;

	if (!node) {
		dev_err(chip->dev, "device tree info. missing\n");
		return -EINVAL;
	}

	rc = of_property_read_u32(node, "qcom,iterm_ma_2nd", &chip->limits.iterm_ma);
	if (rc < 0) {
		chip->limits.iterm_ma = 300;
	}

	rc = of_property_read_u32(node, "qcom,recharge-mv_2nd", &chip->limits.recharge_mv);
	if (rc < 0) {
		chip->limits.recharge_mv = 121;
	}

	rc = of_property_read_u32(node, "qcom,temp_little_cold_vfloat_mv_2nd",
			&chip->limits.temp_little_cold_vfloat_mv);
	if (rc < 0) {
		chip->limits.temp_little_cold_vfloat_mv = 4391;
	}

	rc = of_property_read_u32(node, "qcom,temp_cool_vfloat_mv_2nd",
			&chip->limits.temp_cool_vfloat_mv);
	if (rc < 0) {
		chip->limits.temp_cool_vfloat_mv = 4391;
	}

	rc = of_property_read_u32(node, "qcom,temp_little_cool_vfloat_mv_2nd",
			&chip->limits.temp_little_cool_vfloat_mv);
	if (rc < 0) {
		chip->limits.temp_little_cool_vfloat_mv = 4391;
	}

	rc = of_property_read_u32(node, "qcom,temp_normal_vfloat_mv_2nd",
			&chip->limits.temp_normal_vfloat_mv);
	if (rc < 0) {
		chip->limits.temp_normal_vfloat_mv = 4391;
	}

	rc = of_property_read_u32(node, "qcom,little_cold_vfloat_over_sw_limit_2nd",
			&chip->limits.little_cold_vfloat_over_sw_limit);
	if (rc < 0) {
		chip->limits.little_cold_vfloat_over_sw_limit = 4395;
	}

	rc = of_property_read_u32(node, "qcom,cool_vfloat_over_sw_limit_2nd",
			&chip->limits.cool_vfloat_over_sw_limit);
	if (rc < 0) {
		chip->limits.cool_vfloat_over_sw_limit = 4395;
	}

	rc = of_property_read_u32(node, "qcom,little_cool_vfloat_over_sw_limit_2nd",
			&chip->limits.little_cool_vfloat_over_sw_limit);
	if (rc < 0) {
		chip->limits.little_cool_vfloat_over_sw_limit = 4395;
	}

	rc = of_property_read_u32(node, "qcom,normal_vfloat_over_sw_limit_2nd",
			&chip->limits.normal_vfloat_over_sw_limit);
	if (rc < 0) {
		chip->limits.normal_vfloat_over_sw_limit = 4395;
	}

	rc = of_property_read_u32(node, "qcom,default_iterm_ma_2nd",
			&chip->limits.default_iterm_ma);
	if (rc < 0) {
		chip->limits.default_iterm_ma = 300;
	}

	rc = of_property_read_u32(node, "qcom,default_temp_normal_vfloat_mv_2nd",
			&chip->limits.default_temp_normal_vfloat_mv);
	if (rc < 0) {
		chip->limits.default_temp_normal_vfloat_mv = 4391;
	}

	rc = of_property_read_u32(node, "qcom,default_normal_vfloat_over_sw_limit_2nd",
			&chip->limits.default_normal_vfloat_over_sw_limit);
	if (rc < 0) {
		chip->limits.default_normal_vfloat_over_sw_limit = 4395;
	}

	chg_err("iterm_ma=%d, recharge_mv=%d, temp_little_cold_vfloat_mv=%d, \
temp_cool_vfloat_mv=%d, temp_little_cool_vfloat_mv=%d, \
temp_normal_vfloat_mv=%d, little_cold_vfloat_over_sw_limit=%d, \
cool_vfloat_over_sw_limit=%d, little_cool_vfloat_over_sw_limit=%d, \
normal_vfloat_over_sw_limit=%d, default_iterm_ma=%d, \
default_temp_normal_vfloat_mv=%d, default_normal_vfloat_over_sw_limit=%d\n",
			chip->limits.iterm_ma, chip->limits.recharge_mv, chip->limits.temp_little_cold_vfloat_mv,
			chip->limits.temp_cool_vfloat_mv, chip->limits.temp_little_cool_vfloat_mv,
			chip->limits.temp_normal_vfloat_mv, chip->limits.little_cold_vfloat_over_sw_limit,
			chip->limits.cool_vfloat_over_sw_limit, chip->limits.little_cool_vfloat_over_sw_limit,
			chip->limits.normal_vfloat_over_sw_limit, chip->limits.default_iterm_ma,
			chip->limits.default_temp_normal_vfloat_mv, chip->limits.default_normal_vfloat_over_sw_limit);

	return rc;
}

//====================================================================//
#ifdef CONFIG_OPPO_CHARGER_MT6370_TYPEC
/* Jianchao.Shi@PSW.BSP.CHG.Basic, 2019/07/05, sjc Add for mmi status */
int oppo_chg_get_mmi_status(void)
{
	struct oppo_chg_chip *chip = g_oppo_chip;

	if (!chip) {
		printk(KERN_ERR "[OPPO_CHG][%s]: oppo_chip not ready!\n", __func__);
		return 1;
	}
	if (chip->mmi_chg == 0)
		printk(KERN_ERR "[OPPO_CHG][%s]: mmi_chg[%d]\n", __func__, chip->mmi_chg);
	return chip->mmi_chg;
}
EXPORT_SYMBOL(oppo_chg_get_mmi_status);
#endif /* CONFIG_OPPO_CHARGER_MT6370_TYPEC */
//====================================================================//
#ifdef ODM_WT_EDIT
/*Shouli.Wang@ODM_WT.BSP.CHG 2019/10/07, add custom battery node*/
static ssize_t show_StopCharging_Test(struct device *dev,struct device_attribute *attr, char *buf)
{
	g_oppo_chip->stop_chg = false;
	oppo_chg_turn_off_charging(g_oppo_chip);
	printk("StopCharging_Test\n");
	return sprintf(buf, "chr=%d\n", g_oppo_chip->stop_chg);
}

static ssize_t store_StopCharging_Test(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    return -1;
}
static DEVICE_ATTR(StopCharging_Test, 0664, show_StopCharging_Test, store_StopCharging_Test);

static ssize_t show_StartCharging_Test(struct device *dev,struct device_attribute *attr, char *buf)
{
	g_oppo_chip->stop_chg = true;
	oppo_chg_turn_on_charging(g_oppo_chip);
	printk("StartCharging_Test\n");
	return sprintf(buf, "chr=%d\n", g_oppo_chip->stop_chg);
}
static ssize_t store_StartCharging_Test(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    return -1;
}
static DEVICE_ATTR(StartCharging_Test, 0664, show_StartCharging_Test, store_StartCharging_Test);
#endif /*ODM_WT_EDIT*/
static int oppo_charger_probe(struct platform_device *pdev)
{
#ifdef VENDOR_EDIT
       /* Jianchao.Shi@BSP.CHG.Basic, 2016/12/26, sjc Add for charging*/
        int ret = 0;
        struct oppo_chg_chip *oppo_chip = NULL;
        //struct mt_charger *mt_chg = NULL;
        printk("oppo_charger_probe\n");
        oppo_chip = devm_kzalloc(&pdev->dev,sizeof(struct oppo_chg_chip), GFP_KERNEL);
        if (!oppo_chip) {
            chg_err(" kzalloc() failed\n");
            return -ENOMEM;
        }
        oppo_chip->dev = &pdev->dev;

        ret = oppo_power_supply_init(oppo_chip);

        ret = oppo_chg_parse_svooc_dt(oppo_chip);
        if (oppo_chip->vbatt_num == 1) {
            if (oppo_gauge_check_chip_is_null()) {
                chg_err("gauge chip null, will do after bettery init.\n");
                return -EPROBE_DEFER;
            }
#ifdef ODM_WT_EDIT
////Junbo.Guo@ODM_WT.BSP.CHG, 2019/11/11, Modify for subcharger
		oppo_chip->is_double_charger_support = 0;
		if(is_project(OPPO_19747)) {
			oppo_chip->is_double_charger_support = 1;
			oppo_chip->chg_ops = &oppo_chg_rt9467_ops;
			oppo_chip->sub_chg_ops = &oppo_chg_rt9471_ops;		
		} else {
		   oppo_chip->chg_ops = &oppo_chg_rt9471_ops;
		}
#else
            if (oppo_which_charger_ic() == -1) {
                chg_err("charger IC is null, will do after bettery init.\n");
                return -EPROBE_DEFER;
            } else if (CHG_BQ25601D == oppo_which_charger_ic()) {
			oppo_chip->chg_ops = &bq25601d_chg_ops;
            } else if (CHG_BQ25890H == oppo_which_charger_ic()) {
                oppo_chip->chg_ops = &bq25890h_chg_ops;
            } else if (CHG_BQ24190 == oppo_which_charger_ic()) {
                oppo_chip->chg_ops = &bq24190_chg_ops;
            } else if (CHG_SMB1351 == oppo_which_charger_ic()) {
		#ifdef CONFIG_OPPO_CHARGER_MTK6771
                  oppo_chip->chg_ops = &smb1351_chg_ops;
		#else
		  chg_err("Un supported charger type");
		#endif
            } else if (CHG_MT6370 == oppo_which_charger_ic()) {
		#ifdef CONFIG_OPPO_CHARGER_MT6370_TYPEC
			oppo_chip->chg_ops = &mt6370_chg_ops;
		#else
			chg_err("Un supported charger type");
		#endif
            }
#endif
        } else {
            if (oppo_gauge_ic_chip_is_null() || oppo_vooc_check_chip_is_null()
                    || oppo_adapter_check_chip_is_null()) {
                chg_err("[oppo_chg_init] vooc || gauge || chg not ready, will do after bettery init.\n");
                return -EPROBE_DEFER;
            }
            //oppo_chip->chg_ops = (oppo_get_chg_ops());
        }
        printk("oppo_charger_probe end %p, prev %p, next %p\n",
               &oppo_chip->batt_psy->dev.power.wakeup->entry,
               oppo_chip->batt_psy->dev.power.wakeup->entry.prev,
               oppo_chip->batt_psy->dev.power.wakeup->entry.next);

        g_oppo_chip = oppo_chip;
        oppo_chip->chg_ops->hardware_init();
        /*keep "Ibus < 200mA" for 1s, so vooc/svooc adapter go into idle and release D+*/
        if (oppo_which_charger_ic() == 1) {//for bq25890h
		oppo_chip->chg_ops->input_current_write(100);
        }
        oppo_chip->authenticate = oppo_gauge_get_batt_authenticate();
        oppo_chg_parse_custom_dt(oppo_chip);
        oppo_chg_parse_charger_dt(oppo_chip);
        oppo_chg_usbtemp_parse_dt(oppo_chip);
        if (oppo_which_charger_ic() == 2) {//for bq25601d
		oppo_chg_parse_charger_dt_2nd_override(oppo_chip);
        }
        oppo_chg_init(oppo_chip);
        platform_set_drvdata(pdev, oppo_chip);
	    device_init_wakeup(&pdev->dev, 1);
        oppo_chg_wake_update_work();
        oppo_tbatt_power_off_task_init(oppo_chip);
	if (oppo_usbtemp_check_is_support() == true)
		oppo_usbtemp_thread_init();
#ifdef ODM_WT_EDIT
/*Sidong.Zhao@ODM_WT.BSP.CHG 2019/11/15,add sysfs node for charge control*/
	if (IS_ERR(oppo_chip->batt_psy) == 0) {
		ret = device_create_file(&oppo_chip->batt_psy->dev, &dev_attr_StopCharging_Test);//stop charging
		ret = device_create_file(&oppo_chip->batt_psy->dev, &dev_attr_StartCharging_Test);
	}
#endif /*ODM_WT_EDIT*/

        return 0;
#endif

}

static int oppo_charger_remove(struct platform_device *dev)
{
	return 0;
}

static void oppo_charger_shutdown(struct platform_device *dev)
{
	if (g_oppo_chip != NULL)
		enter_ship_mode_function(g_oppo_chip);
}

static const struct of_device_id oppo_charger_of_match[] = {
	{.compatible = "mediatek,oppo-charger",},
	{},
};

MODULE_DEVICE_TABLE(of, oppo_charger_of_match);

struct platform_device oppo_charger_device = {
	.name = "oppo_charger",
	.id = -1,
};

static struct platform_driver oppo_charger_driver = {
	.probe = oppo_charger_probe,
	.remove = oppo_charger_remove,
	.shutdown = oppo_charger_shutdown,
	.driver = {
		   .name = "oppo_charger",
		   .of_match_table = oppo_charger_of_match,
		   },
};


static int __init oppo_charger_init(void)
{	
	return platform_driver_register(&oppo_charger_driver);
}
late_initcall(oppo_charger_init);


static void __exit mtk_charger_exit(void)
{
	platform_driver_unregister(&oppo_charger_driver);
}
module_exit(mtk_charger_exit);


MODULE_AUTHOR("wy.chuang <wy.chuang@mediatek.com>");
MODULE_DESCRIPTION("MTK Charger Driver");
MODULE_LICENSE("GPL");

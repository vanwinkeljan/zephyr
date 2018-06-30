/*
 * Copyright (c) 2018 dXplore
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SENSOR_TSL2591_H__
#define __SENSOR_TSL2591_H__

#include <zephyr/types.h>
#include <device.h>
#include <i2c.h>

#define TSL2591_ADDR 0x29

/* secondary address can be used to access read-only registers */
#define TSL2591_ADDR_SECONDARY 0x28

#define TSL2591_CMD 0x80

#define TSL2591_CMD_NORMAL  0x20
#define TSL2591_CMD_SEPCIAL 0x60

/* Normal commands */
#define TSL2591_CMD_ENABLE   0x00
#define TSL2591_CMD_CONTROL  0x01
#define TSL2591_CMD_AILTL    0x04
#define TSL2591_CMD_AILTH    0x05
#define TSL2591_CMD_AIHTL    0x06
#define TSL2591_CMD_AIHTH    0x07
#define TSL2591_CMD_NPAILTL  0x08
#define TSL2591_CMD_NPAILTH  0x09
#define TSL2591_CMD_NPAIHTL  0x0A
#define TSL2591_CMD_NPAIHTH  0x0B
#define TSL2591_CMD_PERSIST  0x0C
#define TSL2591_CMD_PID      0x11
#define TSL2591_CMD_ID       0x12
#define TSL2591_CMD_STATUS   0x13
#define TSL2591_CMD_C0DATAL  0x14
#define TSL2591_CMD_C0DATAH  0x15
#define TSL2591_CMD_C1DATAL  0x16
#define TSL2591_CMD_C1DATAH  0x17

/* Special commands */
#define TSL2591_CMD_FORCE_IRQ      0x04
#define TSL2591_CMD_CLEAR_ALL_IRQS 0x06
#define TSL2591_CMD_CLEAR_IRQ      0x07
#define TSL2591_CMD_CLEAR_NPIRQ    0x0A

/* Enable Register */
#define TSL2591_ENA_NPIEN 0x80
#define TSL2591_ENA_SAI   0x40
#define TSL2591_ENA_AIEN  0x10
#define TSL2591_ENA_AEN   0x02
#define TSL2591_ENA_PON   0x01

/* Control Register */
#define TSL2591_CTL_SRESET       0x80
#define TSL2591_CTL_AGAIN_LOW    0x00
#define TSL2591_CTL_AGAIN_MEDIUM 0x10
#define TSL2591_CTL_AGAIN_HIGH   0x20
#define TSL2591_CTL_AGAIN_MAX    0x30
#define TSL2591_CTL_ATIME_100    0x00
#define TSL2591_CTL_ATIME_200    0x01
#define TSL2591_CTL_ATIME_300    0x02
#define TSL2591_CTL_ATIME_400    0x03
#define TSL2591_CTL_ATIME_500    0x04
#define TSL2591_CTL_ATIME_600    0x05

/* Persist Register */
#define TSL2591_APERS_CYCLE 0x00
#define TSL2591_APERS_1     0x01
#define TSL2591_APERS_2     0x02
#define TSL2591_APERS_3     0x03
#define TSL2591_APERS_5     0x04
#define TSL2591_APERS_10    0x05
#define TSL2591_APERS_15    0x06
#define TSL2591_APERS_20    0x07
#define TSL2591_APERS_25    0x08
#define TSL2591_APERS_30    0x09
#define TSL2591_APERS_35    0x0A
#define TSL2591_APERS_40    0x0B
#define TSL2591_APERS_45    0x0C
#define TSL2591_APERS_50    0x0D
#define TSL2591_APERS_55    0x0E
#define TSL2591_APERS_60    0x0F

/* Status Register */
#define TSL2591_STAT_NPINTR 0x20
#define TSL2591_STAT_AINTR  0x10
#define TSL2591_STAT_AVLID  0x01


struct tsl2591_data {

	struct device *i2c_master;
	u16_t lux_vis;
	u16_t lux_ir;
	u8_t atime;
	u16_t gain;

};

#define SYS_LOG_DOMAIN "TSL2591"
#define SYS_LOG_LEVEL CONFIG_SYS_LOG_SENSOR_LEVEL
#include <logging/sys_log.h>

#endif /* __SENSOR_TSL2591_H__ */

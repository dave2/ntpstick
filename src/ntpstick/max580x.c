/* Copyright (C) 2015 David Zanetti
 *
 * This file is part of ntpstick
 *
 * ntpsick is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 *
 * ntpstick is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with ntpstick.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntpstick.h"
#include <util/delay.h>
#include <libkakapo/global.h>
#include <libkakapo/errors.h>
#include <libkakapo/twi.h>
#include <avr/pgmspace.h>
#include "max580x.h"
#include "console.h"

#define DAC_ADDR(x) (0x18 | (x & 0x3))

/* universal I2C device ident command */
#define I2C_VER 0b00000000

/* MAX580X commands over I2C */
#define MAX580X_CODE 0b10000000
#define MAX580X_LOAD 0b10010000
#define MAX580X_CODELOAD 0b10100000
/* 0b10110000 is also CODELOAD */
#define MAX580X_RETURN 0b01110000
/* note: additional bits in the lower nibble for REF */
#define MAX580X_REF 0b00100000
#define MAX580X_REF_DRIVE 0b1000
#define MAX580X_REF_ALWAYSON 0b0100
#define MAX580X_REF_EXT 0b0000
#define MAX580X_REF_25V 0b0001
#define MAX580X_REF_20V 0b0010
#define MAX580X_REF_41V 0b0011
/* actual command in the lower three bits of the command byte */
#define MAX580X_CMD 0b00110000
#define MAX580X_CMD_END 0b000
#define MAX580X_CMD_GATE 0b001
#define MAX580X_CMD_CLR 0b100
#define MAX580X_CMD_RST 0b101
/* power setting is top bits of the 2nd data byte */
#define MAX580X_POWER 0b01000000
#define MAX580X_POWER_NORMAL 0b00000000
#define MAX580X_POWER_PD_1K 0b01000000
#define MAX580X_POWER_PD_100K 0b10000000
#define MAX580X_POWER_PD_HIZ 0b11000000
/* config bits are 5-3 in 2nd data byte */
#define MAX580X_CONFIG 0b01010000
#define MAX580X_CONFIG_AUX_GATE 0b00011000
#define MAX580X_CONFIG_AUX_CLEAR 0b00110000
#define MAX580X_CONFIG_AUX_NONE 0b00111000
/* default bits are upper 3 bits of 2nd data byte */
#define MAX580X_DEFAULT 0b01100000
#define MAX580X_DEFAULT_POR 0b00000000
#define MAX580X_DEFAULT_ZERO 0b00100000
#define MAX580X_DEFAULT_MID 0b01000000
#define MAX580X_DEFAULT_FULL 0b01100000
#define MAX580X_DEFAULT_RETURN 0b10000000

/* magic numbers for ident */
#define MAX580X_ID_5803 0x8a
#define MAX580X_ID_5804 0x92
#define MAX580X_ID_5805 0x82

/* query and report on an attached DAC */
error_t max580x_init(twi_portname_t port, uint8_t addr) {
    uint8_t buf[3], n = 0;

    buf[0] = I2C_VER; buf[1] = 0; buf[2] = 0;
//    twi_write_nostop(port,DAC_ADDR(addr),buf,1);
//    twi_read_restart(port,DAC_ADDR(addr),buf,2);

    twi_write(port,DAC_ADDR(addr),buf,1,0);
    twi_read(port,DAC_ADDR(addr),buf,2,1);

    /* buf should contain some interesting bytes */

    switch (buf[1]) {
        case MAX580X_ID_5803:
            console_message(0,"max580x: Found MAX5803 8-bit DAC");
            break;
        case MAX580X_ID_5804:
            console_message(0,"max580x: Found MAX5804 10-bit DAC");
            break;
        case MAX580X_ID_5805:
            console_message(0,"max580x: Found MAX5805 12-bit DAC");
            break;
        default:
            console_message(0,"max580x: No DAC found");
            return -ENODEV;
            break;
    }

    /* reset, and then power down output until we're ready */
    console_message(0,"max580x: reset, output disable");
//    buf[0] = MAX580X_CMD | MAX580X_CMD_RST;
//    buf[1] = 0; buf[2] = 0;
//    twi_write(port,DAC_ADDR(addr),buf,3);
//    twi_read(port,DAC_ADDR(addr),buf,3);


    buf[0] = MAX580X_POWER;
    buf[1] = 0; buf[2] = MAX580X_POWER_PD_HIZ;

    twi_write(port,DAC_ADDR(addr),buf,3,1);
    twi_read(port,DAC_ADDR(addr),buf,3,1);



    /* configure !AUX to have no special function */
    console_message(0,"max580x: AUXn set to no function");
    buf[0] = MAX580X_CONFIG;
    buf[1] = 0;
    buf[2] = MAX580X_CONFIG_AUX_NONE;
    twi_write(port,DAC_ADDR(addr),buf,3,1);
    twi_read(port,DAC_ADDR(addr),buf,3,1);
    printf("max580x: readback %02x%02x\r\n",buf[1],buf[2]);

    /* end the gate */
    buf[0] = MAX580X_CMD | MAX580X_CMD_END;
    buf[1] = 0;
    buf[2] = 0;
    twi_write(port,DAC_ADDR(addr),buf,3,1);
    twi_read(port,DAC_ADDR(addr),buf,3,1);


    console_message(0,"max580x: config (ext ref, midscale, output en");
    /* ensure reference is set to external */
    buf[0] = MAX580X_REF | MAX580X_REF_EXT;
    buf[1] = 0; buf[2] = 0;
    twi_write(port,DAC_ADDR(addr),buf,3,1);
    twi_read(port,DAC_ADDR(addr),buf,3,1);



    /* set to mid-scale */
    //buf[0] = MAX580X_CODELOAD;
    //buf[1] = 0x80; buf[2] = 0;
    //twi_write(port,DAC_ADDR(addr),buf,3);



    /* output enable */
    buf[0] = MAX580X_POWER;
    buf[1] = 0; buf[2] = MAX580X_POWER_NORMAL;
    twi_write(port,DAC_ADDR(addr),buf,3,1);
    twi_read(port,DAC_ADDR(addr),buf,3,1);


    buf[0] = 0xf0; /* made up value for command */
    twi_write(port,DAC_ADDR(addr),buf,1,0);
    twi_read(port,DAC_ADDR(addr),buf,2,1);
    printf("max580x: status %02x %02x\r\n",buf[0],buf[1]);

    return 0;
}

/* set the DAC value to the given 16-bit value */
/* note: the chip will ignore bits it has no support for, so we always
 * accept and write the full 16-bit value */
error_t max580x_set(twi_portname_t port, uint8_t addr, uint16_t value) {
    uint8_t buf[3];
    int ret;

    console_message(0,"max580x: setting value");
    /* copy the right values */
    buf[0] = MAX580X_CODE;
    buf[1] = ((value & 0xff00) >> 8);
    buf[2] = (value & 0x00ff);
    ret = twi_write(port,DAC_ADDR(addr),buf,3,1);
    if (ret) {
        console_message(0,"max580x: failed DAC update");
        return -EINVAL;
    }
    /* do a read-back */
    twi_read(port,DAC_ADDR(addr),buf,3,1);
    printf("max580x: readback %02x%02x\r\n",buf[1],buf[2]);

    return 0;
}

uint16_t max580x_read(twi_portname_t port, uint8_t addr) {
    uint8_t buf[3];

    buf[0] = MAX580X_CODE;
    twi_write(port,DAC_ADDR(addr),buf,1,0);
    twi_read(port,DAC_ADDR(addr),buf,2,1);

    return ((buf[1] << 8) | buf[0]);
}

int max580x_twi_write(uint8_t addr, uint8_t *buf, uint8_t len, uint8_t nostop) {
    uint8_t n = 0;

    /* keep looping around until we get the bus or attempts max reached */
    while (n++ < 10) {
        printf_P(PSTR("wait idle\r\n"));

        while ((TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) != TWI_MASTER_BUSSTATE_IDLE_gc);

        printf_P(PSTR("write %02x\r\n"),addr);

        TWIC.MASTER.ADDR = (addr << 1);

        while ((TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm) == 0);

        printf_P(PSTR("WIF set\r\n"));

        /* at this point, we need to check what the status is */
        if ((TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_BUSY_gc) {
            /* we failed to acquire the bus for some reason, try again */
            printf_P(PSTR("i2c busy, trying again\r\n"));
            continue;
        }

        /* we got a NAK, then fail the whole transaction */
        if (TWIC.MASTER.STATUS & TWI_MASTER_RXACK_bm) {
            printf_P(PSTR("no device ack'd, giving up\r\n"));
            /* issue a stop to release the bus */
            TWIC.MASTER.CTRLC |= TWI_MASTER_CMD_STOP_gc;
            return -EIO;
        }

        if ((TWIC.MASTER.STATUS & TWI_MASTER_RXACK_bm) == 0) {
            printf_P(PSTR("dev ack (try %d)\r\n"),n);
            break;
        }

        printf_P(PSTR("protocol error, giving up\r\n"));
        TWIC.MASTER.CTRLC |= TWI_MASTER_CMD_STOP_gc;
        return -EIO;
    }

    /* now we should be able to write the data */
    while (len) {
        /* write a byte of data to the bus, and wait for interrupt */
        printf_P(PSTR("out: %02x"),*buf);
        TWIC.MASTER.DATA = *buf;

        buf++;
        len--;

        while ((TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm) == 0);

        printf_P(PSTR("WIF set\r\n"));

        if ((TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_BUSY_gc) {
            /* give up, something else is trashing the bus */
            printf_P(PSTR("i2c became busy, giving up\r\n"));
            return -EIO;
        }

        if (TWIC.MASTER.STATUS & TWI_MASTER_RXACK_bm) {
            /* we got a NAK, so in either case, we stop sending stuff */
            printf_P(PSTR("NAK recieved\r\n"));
            if (!nostop) {
                printf_P(PSTR("issuing stop\r\n"));
                TWIC.MASTER.CTRLC |= TWI_MASTER_CMD_STOP_gc;
            }
            if (len) {
                /* we had bytes left to send */
                return -EIO;
            }
            return 0; /* we had nothing left to send anyway, so we're fine */
        }

        if ((TWIC.MASTER.STATUS & TWI_MASTER_RXACK_bm) == 0) {
            /* all good */
            printf_P(PSTR("slaves wants more\r\n"));
            continue;
        }

        printf_P(PSTR("protocol error, giving up\r\n"));
        TWIC.MASTER.CTRLC |= TWI_MASTER_CMD_STOP_gc;
        return -EIO;
    }

    printf_P(PSTR("wrote everything\r\n"));
    if (!nostop) {
        printf_P(PSTR("issuing stop\r\n"));
        TWIC.MASTER.CTRLC |= TWI_MASTER_CMD_STOP_gc;
    }
    return 0;
}

int max580x_twi_read(uint8_t addr, uint8_t *buf, uint8_t len, uint8_t nostop) {
    uint8_t n = 0;

    /* keep looping around until we get the bus or attempts max reached */
    while (n++ < 10) {

        printf_P(PSTR("wait idle\r\n"));

        while ((TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) != TWI_MASTER_BUSSTATE_IDLE_gc &&
        (TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) != TWI_MASTER_BUSSTATE_OWNER_gc);

        printf_P(PSTR("read %02x\r\n"),addr);

        TWIC.MASTER.ADDR = (addr << 1) | 0x1;

        while ((TWIC.MASTER.STATUS & (TWI_MASTER_WIF_bm | TWI_MASTER_RIF_bm)) == 0);

        printf_P(PSTR("WIF or RIF set\r\n"));

        /* at this point, we need to check what the status is */
        if ((TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_BUSY_gc) {
            /* we failed to acquire the bus for some reason, try again */
            printf_P(PSTR("i2c busy, trying again\r\n"));
            continue;
        }

        /* we got a NAK, then fail the whole transaction */
        if (TWIC.MASTER.STATUS & TWI_MASTER_RXACK_bm) {
            printf_P(PSTR("no device ack'd, giving up\r\n"));
            /* issue a stop to release the bus */
            TWIC.MASTER.CTRLC |= TWI_MASTER_CMD_STOP_gc;
            return -EIO;
        }

        if ((TWIC.MASTER.STATUS & TWI_MASTER_RXACK_bm) == 0 &&
            TWIC.MASTER.STATUS & TWI_MASTER_RIF_bm) {
            printf_P(PSTR("dev ack, data present (try %d)\r\n"),n);
            break;
        }

        printf_P(PSTR("protocol error, giving up\r\n"));
        TWIC.MASTER.CTRLC |= TWI_MASTER_CMD_STOP_gc;
        return -EIO;
    }

    while (len) {
        *buf = TWIC.MASTER.DATA;
        printf_P(PSTR("rx %02x\r\n"),*buf);
        buf++;
        len--;

        /* we now need to decide what to do */
        if ((TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_BUSY_gc) {
            /* give up, something else is trashing the bus */
            printf_P(PSTR("i2c became busy, giving up\r\n"));
            return -EIO;
        }

        /* if we have data to continue to RX, please provide it */
        if (!len) {
            printf_P(PSTR("we will term\r\n"));
            TWIC.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_RECVTRANS_gc;
            break;
        }

        printf_P(PSTR("we expect more\r\n"));
        TWIC.MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;


        printf_P(PSTR("waiting for next packet\r\n"));
        /* now wait for the next RIF or timeout */
        while ((TWIC.MASTER.STATUS & TWI_MASTER_RIF_bm) == 0 &&
            (TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm) == 0);
    }

    printf_P(PSTR("read everything\r\n"));
    if (!nostop) {
        printf_P(PSTR("issuing stop\r\n"));
        TWIC.MASTER.CTRLC |= TWI_MASTER_CMD_STOP_gc;
    }
    return 0;
}

/* Copyright (C) 2014 David Zanetti
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
 * License along with libkapapo.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include "ntpstick.h"
#include <libkakapo/global.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <libkakapo/clock.h>
#include <libkakapo/usart.h>
#include <libkakapo/errors.h>
#include <libkakapo/spi.h>
#include <libkakapo/twi.h>
#include <libkakapo/net_w5500.h>
#include "console.h"

/* hook to get processing of debug working */
void debugcon_hook(uint8_t c);

volatile uint8_t flags;
#define FLAG_DEBUGCON 0x01

/* global variables needed from eeprom */
/* note: these are placeholders re-written by config_restore() */

/* This is a magic number we can use to detect validity */
uint16_t EEMEM eeprom_magic;
/* IP address configuration */
uint8_t EEMEM eeprom_v4[4];
uint8_t EEMEM eeprom_v4cidr;
uint8_t EEMEM eeprom_v4gw[4];

/* private prototypes */
void config_check(void);

/* This is the main firmware entry point */

void main(void) {
    uint8_t idle;
    FILE *debugcon;

    /* FIXME: rewrite for VCXO init */

    /* enable the 32MHz OSC */
    clock_osc_run(osc_rc32mhz);
    /* switch to it */
    clock_sysclk(sclk_rc32mhz);

    /* enable interrupts */
    PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
 	sei();

    PORTE.DIRSET = PIN2_bm | PIN3_bm;

    flags = 0;
    set_sleep_mode(SLEEP_MODE_IDLE); /* just stop clocking CPU when required to idle */
    config_check();

    /* init some serial ports! */
    usart_init(usart_d0,64,128);
    usart_conf(usart_d0,115200,8,none,1,U_FEAT_NONE,&debugcon_hook);
    usart_run(usart_d0);
    debugcon = usart_map_stdio(usart_d0);

    /* this is required to force sync with the FTDI chip */
    putchar(0);
    _delay_ms(1);

    console_open(0,debugcon);

    printf_P(PSTR("\fNTPstick v1.1\r\nFirmware Build %s\r\nCopyright 2014 David Zanetti\r\nLicensed under GPLv2\r\nhairy.geek.nz/ntp\r\n"),
            BUILDDATE);

    /* commence configuration of the network */
    {
        uint8_t mac[6], ip[4], cidr, gw[4];

        twi_init(twi_e,400);
        twi_write(twi_e,0x50,"\xFA",1);
        twi_read(twi_e,0x50,&mac,6);
        printf_P(PSTR("mac: %02x:%02x:%02x:%02x:%02x:%02x\r\n"),
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        eeprom_read_block(&ip,&eeprom_v4,4);
        eeprom_read_block(&gw,&eeprom_v4gw,4);
        cidr = eeprom_read_byte(&eeprom_v4cidr);

        w5500_init(spi_d,&PORTD,PIN4_bm,mac);
        w5500_ip_conf(ip,cidr,gw);
        printf_P(PSTR("ip: %d.%d.%d.%d/%d (gw %d.%d.%d.%d)\r\n"),
            ip[0], ip[1], ip[2], ip[3], cidr, gw[0], gw[1], gw[2], gw[3]);
    }

    console_set_prompt(0,"ntpstick$ ");
    console_prompt(0);

    idle = 0;
    while (1) {
        if (flags & FLAG_DEBUGCON) {
            flags &= ~(FLAG_DEBUGCON);
            console_process(0);
            //console_message(0,"Got char!");
        }
        idle++;
        cli();
        if (idle > 16) {
            idle = 0;
            /* go to sleep */
            sleep_enable();
            sei();
            sleep_cpu();
            sleep_disable();
        }
    }
}

void debugcon_hook(uint8_t c) {
    flags |= FLAG_DEBUGCON;
}

/* this validates we have a valid configuration in the eeprom */
void config_check(void) {
    /* this will retrieve all required variables from the eeprom */
    /* first, check the validity of the eeprom signature, if it's invalid
     * then write defaults for the various values needed */
    if (eeprom_read_word(&eeprom_magic) != EEPROM_MAGIC) {
        uint8_t addr[4] = {10,32,34,8};
        eeprom_write_block((const void *)&addr,(void *)&eeprom_v4,4);
        eeprom_write_byte(&eeprom_v4cidr,24);
        /* now finish up with the writes for the magic number */
        eeprom_write_word(&eeprom_magic,EEPROM_MAGIC);
    }
}

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
#include <string.h>
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
void telnetd_s0_accept(void);
void telnetd_s0_rx(void);

volatile uint8_t flags;
#define FLAG_DEBUGCON 0x01
#define FLAG_TELNETD0 0x02
#define FLAG_TELNETD1 0x04
#define FLAG_W5500 0x08

FILE *telnetd_s0;
FILE *telnetd_s1;

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

        eeprom_read_block(&ip,&eeprom_v4,4);
        eeprom_read_block(&gw,&eeprom_v4gw,4);
        cidr = eeprom_read_byte(&eeprom_v4cidr);

        w5500_init(spi_d,&PORTD,PIN4_bm,mac);
        w5500_ip_conf(ip,cidr,gw);
    }

    /* configure the interrupt for w5500 events */
    PORTC.DIRCLR = PIN4_bm;
    PORTC.PIN4CTRL = PORT_ISC_FALLING_gc;
    PORTC.INT0MASK |= PIN4_bm;
    PORTC.INTCTRL |= PORT_INT0LVL_LO_gc; /* low prio */

    console_set_prompt(0,"ntpstick$ ");
    console_prompt(0);

    /* set up telnetd */
    w5500_socket_init(0,2,2);
    w5500_tcp_listen(0,23,&telnetd_s0_accept,&telnetd_s0_rx);
    telnetd_s0 = w5500_tcp_map_stdio(0,32);

    idle = 0;
    while (1) {
        if (flags & FLAG_DEBUGCON) {
            flags &= ~(FLAG_DEBUGCON);
            console_process(0);
            //console_message(0,"Got char!");
        }
        if (flags & FLAG_W5500) {
            flags &= ~(FLAG_W5500);
            w5500_poll();
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
        memset(addr,0,4);
        eeprom_write_block((const void *)&addr,(void *)&eeprom_v4gw,4);
        /* now finish up with the writes for the magic number */
        eeprom_write_word(&eeprom_magic,EEPROM_MAGIC);
    }
}

ISR(PORTC_INT0_vect) {
    /* check to see if the level on PC2 is low */
    if (!(PORTC.IN & PIN4_bm)) {
        flags |= FLAG_W5500;
    }
}

void telnetd_s0_accept(void) {
    console_message(0,"telnetd: new connection opened\r\n");
    fprintf_P(telnetd_s0,PSTR("ntpstick-1.1-telnetd\r\n"));
    console_open(1,telnetd_s0);
    console_set_prompt(1,"ntpstick$ ");
    console_prompt(1);
    w5500_tcp_push(0); /* force packets to be sent */
}

void telnetd_s0_rx(void) {
    console_process(1);
    /* force a flush of the packets it may have generated */
    w5500_tcp_push(0);
}

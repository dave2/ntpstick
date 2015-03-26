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
#include <libkakapo/nvm.h>
#include "console.h"
#include "max580x.h"

/* hook to get processing of debug working */
void debugcon_hook(uint8_t c);
void gps_hook(uint8_t c);
void telnetd_hook(uint8_t socket, w5500_event_t event);

volatile uint8_t flags;
#define FLAG_DEBUGCON 0x01
#define FLAG_TELNETD0 0x02
#define FLAG_TELNETD1 0x04
#define FLAG_W5500 0x08
#define FLAG_GPS 0x10
#define FLAG_TELNET_OPEN 0x20

#undef DEBUG_GPS_PASS

#define I2C_PORT_MAC twi_c
#define I2C_PORT_DAC twi_c

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
void spam_version(FILE *where);

uint32_t jenkins_one_at_a_time_hash(uint8_t *key, uint8_t len);
void obtain_mac(twi_portname_t port, uint8_t *mac);

/* This is the main firmware entry point */

void main(void) {
    uint8_t idle;
    FILE *debugcon, *gps;

    /* FIXME: rewrite for VCXO init */

    /* enable the 32MHz OSC */
    clock_osc_run(osc_rc32mhz);
    /* switch to it */
    clock_sysclk(sclk_rc32mhz);

    /* enable interrupts */
    PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
 	sei();

    PORTA.DIRSET = PIN5_bm | PIN6_bm;

    flags = 0;
    set_sleep_mode(SLEEP_MODE_IDLE); /* just stop clocking CPU when required to idle */
    config_check();

    /* init some serial ports! */
    usart_init(usart_d0,64,128);
    usart_conf(usart_d0,921600,8,none,1,U_FEAT_NONE,&debugcon_hook);
    usart_run(usart_d0);
    debugcon = usart_map_stdio(usart_d0);

    /* this is required to force sync with the FTDI chip */
    putchar(0);
    _delay_ms(1);

#ifndef DEBUG_GPS_PASS
    console_open(0,debugcon,ch_mode_serial);
    console_version(debugcon);
#endif

    /* start passing through GPS noise */
    usart_init(usart_e0,256,64);
    usart_conf(usart_e0,9600,8,none,1,U_FEAT_NONE,&gps_hook);
    usart_run(usart_e0);
    gps = usart_map_stdio(usart_e0);

    /* init I2C port for both DAC and MAC EEPROM */
    twi_init(I2C_PORT_MAC,400,200);
    if (I2C_PORT_MAC != I2C_PORT_DAC) {
        twi_init(I2C_PORT_DAC,400,200);
    }

    {
        uint8_t mac[6], ip[4], cidr, gw[4];

        obtain_mac(I2C_PORT_MAC,mac);

        printf("mac: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        eeprom_read_block(&ip,&eeprom_v4,4);
        eeprom_read_block(&gw,&eeprom_v4gw,4);
        cidr = eeprom_read_byte(&eeprom_v4cidr);

        /* do a hard reset */
        PORTC.DIRSET = PIN2_bm;
        PORTC.OUTCLR = PIN2_bm;
        _delay_ms(1);
        PORTC.OUTSET = PIN2_bm;
        _delay_ms(1);

        w5500_init(spi_c,&PORTC,PIN4_bm,mac);
        w5500_ip_conf(ip,cidr,gw);

        /* configure the interrupt for w5500 events */
        PORTC.DIRCLR = PIN3_bm;
        PORTC.PIN3CTRL = PORT_ISC_FALLING_gc;
        PORTC.INT0MASK |= PIN3_bm;
        PORTC.INTCTRL |= PORT_INT0LVL_LO_gc; /* low prio */
    }

    /* find out if the DAC is there */
    max580x_init(twi_c,0);
    max580x_set(twi_c,0,0x7fff);
    //printf("current value is %04x\r\n",max580x_read(twi_c,0));

    console_set_prompt(0,"ntpstick$ ");
    console_prompt(0);


    /* set up telnetd */
    //telnetd_listen(0,1);

    idle = 0;
    while (1) {
        if (flags & FLAG_DEBUGCON) {
            int c;

            flags &= ~(FLAG_DEBUGCON);
#ifndef DEBUG_GPS_PASS
            console_process(0);
#else
            while (1) {
                c = fgetc(debugcon);
                if (c == EOF) {
                    break;
                }
                fputc(c,gps);
            }
#endif
            //console_message(0,"Got char!");
        }
        if (flags & FLAG_GPS) {
            uint16_t x;
            uint8_t c[255], n = 0;

            flags &= ~(FLAG_GPS);
            /* process some GPS stuff, drain for now */
            while (fgetc(gps) != EOF);
        }
        if (flags & FLAG_W5500) {
            flags &= ~(FLAG_W5500);
            while (!(PORTC.IN & PIN3_bm)) {
                w5500_poll();
            }
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

void gps_hook(uint8_t c) {
    if (c == '\n') {
        flags |= FLAG_GPS;
    }
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
    if (!(PORTC.IN & PIN3_bm)) {
        flags |= FLAG_W5500;
    }
}

/* function to populate a MAC address into the given struct */
void obtain_mac(twi_portname_t port, uint8_t *mac) {
    uint8_t n;

    /* first approach, find an I2C eeprom with useful characteristics
     * on the bus, we read from 0xFA-0xFF if we find one
     * any EEPROM will do but this is probably a 24AA02E48 MAC chip */
    for (n = 0x50; n <= 0x57; n++) {
        /* probe for a possible MAC chip */
        if (twi_start(port,n,twi_mode_write)) {
            continue; /* errored out */
        }
        /* have a device hailed, try to identify it */
        twi_write(port,"\xFA",1,twi_more);
        twi_start(port,n,twi_mode_read);
        twi_read(port,mac,6,twi_stop);
        return;
    }

    /* we don't have a MAC externally attached, therefore, make
     * one up */
    /* "locally administrated" MACs are xxxxxx1x in the first octect on
     * the wire. This is actually the 2nd digit if writen in typical hex
     * format, as x2, x6, xA, xE */
    {
        uint8_t serial[11]; /* 11 byte serial number */
        uint32_t hash;

        nvm_serial(serial); /* copy the serial number */
        hash = jenkins_one_at_a_time_hash(serial,11);
        mac[0] = 0x02;
        mac[1] = 0x00;
        mac[2] = (hash >> 24) & 0xff;
        mac[3] = (hash >> 16) & 0xff;
        mac[4] = (hash >> 8) & 0xff;
        mac[5] = (hash) & 0xff;
        printf("mac: [warn] using generated MAC address\r\n");
    }
    return;
}

/* copied largely from http://en.wikipedia.org/wiki/Jenkins_hash_function */
uint32_t jenkins_one_at_a_time_hash(uint8_t *key, uint8_t len)
{
    uint32_t hash;
    uint8_t i;

    for(hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

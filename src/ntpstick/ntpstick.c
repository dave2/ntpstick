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
#include <libkakapo/clock.h>
#include <libkakapo/usart.h>
#include <libkakapo/errors.h>
#include "console.h"

/* hook to get processing of debug working */
void debugcon_hook(uint8_t c);

volatile uint8_t flags;
#define FLAG_DEBUGCON 0x01

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

    /* init some serial ports! */
    usart_init(usart_d0,64,128);
    usart_conf(usart_d0,115200,8,none,1,U_FEAT_NONE,&debugcon_hook);
    usart_run(usart_d0);
    debugcon = usart_map_stdio(usart_d0);

    /* this is required to force sync with the FTDI chip */
    putchar(0);
    _delay_ms(1);

    console_open(0,debugcon);

    printf("\r\nhello, world\r\n");

    console_set_prompt(0,"ntpstick> ");
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

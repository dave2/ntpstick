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
#include <libkakapo/global.h>
#include <libkakapo/errors.h>
#include <libkakapo/twi.h>
#include <avr/pgmspace.h>
#include "max580x.h"
#include "console.h"

#define DAC_ADDR(x) (0x18 | (x & 0x3))

/* query and report on an attached DAC */
error_t max580x_init(twi_portname_t port, uint8_t addr) {
    uint8_t buf[6] = {0b00000000,0,0,0,0,0};

    twi_write_nostop(port,DAC_ADDR(addr),buf,1);
    twi_read(port,DAC_ADDR(addr),buf,2);
    //twi_read(port,DAC_ADDR(addr),buf,3);
    /* buf should contain some interesting bytes */
    if ((buf[0] & 0b11111000) != 0b01010000) {
        console_message(0,"max580x: failed to detect chip\r\n");
        return -ENODEV;
    }
    switch (buf[1]) {
        case 0x8a:
            console_message(0,"max580x: MAX5803 8-bit DAC found");
            break;
        case 0x92:
            console_message(0,"nmax580x: MAX5804 10-bit DAC found");
            break;
        case 0x82:
            console_message(0,"max580x: MAX5805 12-bit DAC found");
            break;
        default:
            console_message(0,"max580x: Unknown DAC found");
            return -ENODEV;
            break;
    }
    return 0;
}

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
#include <avr/pgmspace.h>
#include <libkakapo/errors.h>
#include <libkakapo/spi.h>
#include <libkakapo/net_w5500.h>
#include "telnetd.h"
#include "console.h"

FILE *telnetd[2]; /* file handles for the socket stdio */

/* telnet commands */
#define TELNETD_IAC 255 /* never seen really */
#define TELNETD_DONT 254
#define TELNETD_DO 253
#define TELNETD_WONT 252
#define TELNETD_WILL 251
#define TELNETD_SB 250
#define TELNETD_GA 249
#define TELNETD_EL 248
#define TELNETD_EC 247
#define TELNETD_AYT 246
#define TELNETD_AO 245
#define TELNETD_IP 244
#define TELNETD_BRK 243
#define TELNETD_DATA_MARK 242
#define TELNETD_NOP 241
#define TELNETD_SE 240

/* the full list of options from various RFCs */
#define TELNETD_BINARY 0
#define TELNETD_ECHO 1
#define TELNETD_SUPRESS_GO_AHEAD 3
#define TELNETD_STATUS 5
#define TELNETD_MARK 6
#define TELNETD_NAOCRD 10
#define TELNETD_NAOHTS 11
#define TELNETD_NAOHTD 12
#define TELNETD_NAOFFD 13
#define TELNETD_NAOVTS 14
#define TELNETD_NAOVTD 15
#define TELNETD_NAOLFD 16
#define TELNETD_EXTEND_ASCII 17
#define TELNETD_LOGOUT 18
#define TELNETD_BM 19
#define TELNETD_DET 20
#define TELNETD_SEND_LOCATION 23
#define TELNETD_TERMINAL_TYPE 24
#define TELNETD_END_OF_RECORD 25
#define TELNETD_TUID 26
#define TELNETD_OUTMRK 27
#define TELNETD_TTYLOC 28
#define TELNETD_3270_REGIME 29
#define TELNETD_X3_PAD 30
#define TELNETD_NAWS 31
#define TELNETD_TERMINAL_SPEED 32
#define TELNETD_TOGGLE_FLOW_CONTROL 33
#define TELNETD_LINEMODE 34
#define TELNETD_X_DISPLAY_LOCATION 35
#define TELNETD_ENVIRON 36
#define TELNETD_AUTHENICATION 37
#define TELNETD_ENCRYPT 38
#define TELNETD_NEW_ENVIRON 39
#define TELNETD_TN3270E 40
#define TELNETD_CHARSET 42
#define TELNETD_COM_PORT_OPTION 44
#define TELNETD_KERMIT

void telnetd_hook(uint8_t socket, w5500_event_t event);

/* this function sets up the two sockets we'll need to listen on to get
 * connections to cycle */
int telnetd_listen(uint8_t socka, uint8_t sockb) {
    /* set up telnetd */
    w5500_socket_init(socka,2,2);
    w5500_tcp_listen(socka,23,&telnetd_hook);
    telnetd[0] = w5500_tcp_map_stdio(socka,128);
    w5500_socket_init(sockb,2,2);
    w5500_tcp_listen(sockb,23,&telnetd_hook);
    telnetd[1] = w5500_tcp_map_stdio(sockb,128);

    return 0;
}

/* this function handles all the telnetd commands we expect to see */
int telnetd_command(FILE *stream) {
    switch (fgetc(stream)) {
        case TELNETD_IAC:
            /* ahh, this is an escape for some reason? never mind */
            break;
        case TELNETD_DO:
            /* which option are we dealing with */
            switch (fgetc(stream)) {
                case TELNETD_SUPRESS_GO_AHEAD:
                    /* yes, we'll do this */
                    /* IAC, WILL, SUPRESS-GO-AHEAD */
                    fprintf_P(stream,PSTR("\xff\xfb\x03"));
                    break;
                case TELNETD_STATUS:
                    /* no, we don't bother with this */
                    /* IAC, WON'T, STATUS */
                    fprintf_P(stream,PSTR("\xff\xfe\05"));
                    break;
                default:
                    break;
            }
        case TELNETD_WILL:
            /* these are options we are being offered */
            switch (fgetc(stream)) {
                case TELNETD_LINEMODE:
                    /* no, we don't want line mode */
                    /* IAC, DON'T, LINEMODE */
                    fprintf_P(stream,PSTR("\xff\xfc\x22"));
                    break;
            }
        default:
            break; /* nothing much we care about */
    }
    return 0;
}

/* this hook handles the various IP layer events from the chip */
void telnetd_hook(uint8_t socket, w5500_event_t event) {
    switch (event) {
        /* a new connection on the given socket, accept it */
        case w5500_accept:
            console_message(0,"telnetd: new connection opened");
            /* close all existing sockets */
            console_close(1); /* close the old console for telnetd */
            /* burst the commands we want */
            fprintf_P(telnetd[socket],PSTR("\xff\xfd\x03\xff\xfe\x22\xff\xfc\x05\xff\xfb\x01"));
            w5500_tcp_push(socket);
            console_open(1,telnetd[socket],ch_mode_telnet);
            console_process(1); /* force it to eat whatever we got sent */
            console_set_prompt(1,"ntpstick$ ");
            console_version(telnetd[socket]);
            console_prompt(1);
            w5500_tcp_push(socket); /* force packets to be sent */
            //flags |= FLAG_TELNET_OPEN;
            break;
        case w5500_rx:
            //console_message(0,"telnetd: rx");
            console_process(1);
            /* force a flush of the packets it may have generated */
            w5500_tcp_push(socket);
            break;
        case w5500_dc:
            console_message(0,"telnetd: peer closed connection");
            /* socket is already closed, close console */
            console_close(1);
            //flags &= ~(FLAG_TELNET_OPEN);
            /* make listening socket again */
            w5500_tcp_close(socket); /* force it back into closed state */
            w5500_tcp_listen(socket,23,&telnetd_hook);
            break;
        default:
            console_message(0,"telnetd: unexpected event");
            break;
    }
}

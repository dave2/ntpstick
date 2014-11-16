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

/* the console defines multiple channels to accept commands.. we assume
 * we are given a handle to each stream */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntpstick.h"
#include <libkakapo/global.h>
#include <libkakapo/errors.h>
#include "console.h"

#define CONSOLE_MAX_CMD 64
#define CONSOLE_MAX_PROMPT 32
#define CONSOLE_MAX_CHAN 3

typedef enum {
    ch_input_default = 0, /**< Default range of characters */
    ch_input_ip, /**< Only accept keys which match IP address ranges */
    ch_input_yn, /**< Only accept Y or N for questions */
} ch_input_t;

typedef struct {
    char cmd_buf[CONSOLE_MAX_CMD]; /**< Command buffer for this channel */
    uint8_t cursor; /**< Where is the cursor in the command buffer */
    uint8_t end; /**< Where is the end of the command buffer */
    char prompt_buf[CONSOLE_MAX_PROMPT]; /**< Prompt for this channel */
    FILE *stream; /**< stdio stream associated with it */
    ch_input_t accept; /**< what chars we are accepting at this time */
} console_t;

console_t chan[CONSOLE_MAX_CHAN];

/* process any command */
void console_command(uint8_t chanid);

/* Channel open is passed a stdio file, and starts accepting chars. */
error_t console_open(uint8_t chanid, FILE *stream) {
    /* check to see we're not out of range */
    if (chanid >= CONSOLE_MAX_CHAN) {
        return -ENODEV;
    }

    if (!stream) {
        return -EINVAL;
    }

    /* now reset the various pointers into the buffer */
    chan[chanid].cursor = 0;
    chan[chanid].end = 0;
    memset(chan[chanid].cmd_buf,0,CONSOLE_MAX_CMD);
    memset(chan[chanid].prompt_buf,0,CONSOLE_MAX_PROMPT);
    chan[chanid].stream = stream;
    chan[chanid].accept = ch_input_default;

    /* don't output prompt since we have none! */

    return 0;
}

/* close a console. I guess we should flush the stdio channel or
 * something? */
error_t console_close(uint8_t chanid) {
    /* check to see we're not out of range */
    if (chanid >= CONSOLE_MAX_CHAN) {
        return -ENODEV;
    }

    /* generally this doesn't do anything at this point */
    return 0;
}

/* set a console prompt */
error_t console_set_prompt(uint8_t chanid, char *prompt) {
    uint16_t prompt_len;

    if (chanid >= CONSOLE_MAX_CHAN) {
        return -ENODEV;
    }

    /* clear any existing prompt */
    memset(chan[chanid].prompt_buf,0,CONSOLE_MAX_PROMPT);

    /* set one if we have one provided */
    if (prompt) {
        prompt_len = strlen(prompt);
        if (prompt_len >= CONSOLE_MAX_PROMPT - 1) {
            prompt_len = CONSOLE_MAX_PROMPT - 1;
        }
        memcpy(chan[chanid].prompt_buf,prompt,prompt_len);
    }

    return 0;
}

/* prompt for a response */
error_t console_prompt(uint8_t chanid) {
    if (chanid >= CONSOLE_MAX_CHAN) {
        return -ENODEV;
    }

    chan[chanid].cursor = 0;
    chan[chanid].end = 0;
    memset(chan[chanid].cmd_buf,0,CONSOLE_MAX_CMD);
    /* now output the prompt on a new line */
    fprintf(chan[chanid].stream,"\r\n%s",chan[chanid].prompt_buf);
    /* eat all the input characters to clear any trash in there */
    while (fgetc(chan[chanid].stream) != EOF);
    /* null body */
    return 0;
}

error_t console_process(uint8_t chanid) {
    int c;

    if (chanid >= CONSOLE_MAX_CHAN) {
        return -ENODEV;
    }

    /* keep processing chars until we run out */
    while (1) {
        c = fgetc(chan[chanid].stream);
        if (c == EOF) {
            break;
        }

        /* now work out what we're doing with it */
        if ((c >= 0x20 && c <= 0x7e)) {
            if (chan[chanid].end < CONSOLE_MAX_CMD) {
                /* add it to the command buffer */
                chan[chanid].cmd_buf[chan[chanid].end] = c;
                chan[chanid].end++;
                fputc(c,chan[chanid].stream);
            } else {
                fputc('\a',chan[chanid].stream);
            }
        }
        if (c == 0x8 || c == 0x7f) {
            /* delete! */
            if (chan[chanid].end == 0) {
                fputc('\a',chan[chanid].stream);
            } else {
                chan[chanid].cmd_buf[chan[chanid].end] = 0;
                chan[chanid].end--;
                fprintf(chan[chanid].stream,"\b \b");
            }
        }

        if (c == '\r') {
            /* log a message containing the result */
            if (chan[chanid].end > 0) {
                fprintf(chan[chanid].stream,"\r\n%s",chan[chanid].cmd_buf);
                /* examine the command for what the result could be */
                console_command(chanid);
            }
            console_prompt(chanid);
        }
    }
    return 0;
}

/* display a message */
error_t console_message(uint8_t chanid, char *message) {
    if (chanid >= CONSOLE_MAX_CHAN) {
        return -ENODEV;
    }
    /* print message */
    fprintf(chan[chanid].stream,"\e[2K\r%s\r\n",message);
    /* re-present prompt */
    fprintf(chan[chanid].stream,"%s",chan[chanid].prompt_buf);
    /* FIXME: and the current command buffer state, and cursor position */
    return 0;
}

/* command processing actually takes place here */
void console_command(uint8_t chanid) {
    return;
}

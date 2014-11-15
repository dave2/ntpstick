#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

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

/** \brief Open a console channel, given an stdio handle for it
 *
 *  This will initalise the buffers and associate the given file
 *  handle with the channel.
 *
 *  \param chanid What channel number to open (0-n)
 *  \param stream Pointer to the stdio stream to read/write from
 *  \return 0 on success, libkakapo/errors.h otherwise
 */
error_t console_open(uint8_t chanid, FILE *stream);

/** \brief Close the channel.
 *
 *  This ensures any writes to the channel are discarded properly.
 *
 *  \param chanid What channel number to close (0-n)
 *  \return 0 on success, libkakapo/errors.h otherwise
 */
error_t console_close(uint8_t chanid);

/** \brief Process characters coming in for a channel
 *
 *  This should be called on a regular basis, or when the lower level
 *  has indicated it has characters.
 *
 *  \param chanid What channel number to process (0-n)
 *  \return 0 on sucess, libkakapo/errors.h otherwise
 */
error_t console_process(uint8_t chanid);

/** \brief Set the prompt to be used for accepting commands
 *
 *  This is what is used as the command line prompt for the console.
 *  The whole prompt must be provided, there are no implied characters
 *  added to the end (ie, no shell indicator or space)
 *
 *  \param chanid What channel number this is set on (0-n)
 *  \param prompt String to use as a prompt.
 *  \return 0 on success, libkakapo/errors.h otherwise
 */
error_t console_set_prompt(uint8_t chanid, char *prompt);

/** \brief Prompt for a command
 *
 *  This is the step which actually outputs the prompt and begins to
 *  accept commands. The actual command processing takes place in
 *  console_process().
 *
 *  \param chanid What channel to accept a command on
 *  \return 0 on success, libkakapo/errors.h otherwise
 */
error_t console_prompt(uint8_t chanid);

/** \brief Output a log message to the console
 *
 *  This erases the current line output, puts the log messages and
 *  the restores the command state (if any)
 *
 *  \param chanid What channel to output log to
 *  \param message Message string to post
 *  \return 0 on success, libkakapo/errors.h otherwise
 */
error_t console_message(uint8_t chanid, char *message);

#endif // CONSOLE_H_INCLUDED

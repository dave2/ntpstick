#ifndef TELNETD_H_INCLUDED
#define TELNETD_H_INCLUDED

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

/** \brief Start listening for telnet connections on the two sockets given
 *
 *  \param socka First socket to listen on
 *  \param sockb Second socket to listen on
 *  \return 0 on success, libkakapo/errors.h otherwise
 */
int telnetd_listen(uint8_t socka, uint8_t sockb);

/** \brief Stop listening for telnet connections
 *
 *  \return 0 on success, libkakapo/errors.h otherwise
 */
int telnetd_close(void);

/** \brief Process a given command obtained from the connection
 *
 *  \param cmd Command byte
 *  \param value Value byte
 *  \return 0 on success, libkakapo/errors.h otherwise
 */
int telnetd_command(FILE *stream);

#endif // TELNETD_H_INCLUDED

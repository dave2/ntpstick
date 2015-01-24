#ifndef MAX580X_H_INCLUDED
#define MAX580X_H_INCLUDED

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

/* \file
 * \brief MAX5803/5804/5805 I2C DAC driver
 *
 * All use the same protocol, but are 8, 10, or 12 bit precsion.
 * Precision is hidden such that a 16-bit value is written and
 * only top n bits are considered. Therefore we accept 16-bit
 * values for the DAC setting */

/** \brief Initalise a MAX580x
 *
 * This ensures the DAC is detected correctly and may report type
 *
 * \param port Which I2C port to use
 * \param addr The bottom two address select bits (NOT the full adddress)
 * \return 0 on success, errors.h otherwise
 */
error_t max580x_init(twi_portname_t port, uint8_t addr);

/** \brief Set the DAC to a specific value
 *
 * Pass a 16-bit value to the DAC to set as output. Note that the actual
 * precision will be less than this determined by the device type. The
 * top n bits are used for actual DAC setting, so the full range is 0-0xffff
 * regardless of precision.
 *
 * \param port Which I2C port to use
 * \param addr The bottom two address select bits (NOT the full address)
 * \param value The 16-bit value to set to the DAC to
 * \return 0 on success (readback from DAC), errors.h otherwise
 */
error_t max580x_set(twi_portname_t port, uint8_t addr, uint16_t value);

#endif // MAX580X_H_INCLUDED

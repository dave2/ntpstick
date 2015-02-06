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
 * License along with libkapapo.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

/* An implementation of a Digital Frequency Locked Loop (DFLL) */

/* Given a 1Hz reference pulse on a specific pin, set up a timer to count
 * how many pulses we get from the oscillator, and tune an attached DAC
 * according to whether the frequency of the oscillator is fast or slow.
 *
 *  GPS-->ref1hz-->compare-->adjust_dac-.
 *                    ^                  |
 *                    `--oscillator<-----'
 */

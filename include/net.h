/**
 *
 * Copyright (c) 2013 Thomas Sader (thommey)
 *
 *  This file is part of PLservices.
 *
 *  PLservices is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PLservices is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PLservices.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#ifndef NET_H_
#define NET_H_

void net_connect(void);
void send_format(const char *format, ...);
void send_words(const char *first, ...);
void send_raw(char *str);
void net_read(void);

#define send_raw(s) send_raw(s "\r\n")
#define send_format(f, ...) send_format(f "\r\n", __VA_ARGS__)
#define send_words(...) send_words(__VA_ARGS__, NULL)

#endif /* NET_H_ */

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
**/

#ifndef CONVERT_H_
#define CONVERT_H_

struct user *convert_nick(char *str);
struct user *convert_unum(char *str);
struct server *convert_snum(char *str);
struct entity *convert_num(char *str);
struct channel *convert_chan(char *str);
unsigned int *convert_uint(unsigned int *dest, char *str);
long *convert_long(long *dest, char *str);
time_t *convert_time(time_t *dest, char *str);
void free_conversion(void);

#endif // CONVERT_H_

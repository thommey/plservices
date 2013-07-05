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

#ifndef TOKENS_H_
#define TOKENS_H_

void add_token(char *shortname, char *name, char *rulestr1, char *rulestr2, void (*handler1)(void), void (*handler2)(void));
void init_tokens(void);
void memreport_tokens(void);
void handle_input(char *str);
void set_registered(int reg);

#endif // TOKENS_H_

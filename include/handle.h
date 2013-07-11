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

#ifndef HANDLE_H_
#define HANDLE_H_

void hACCOUNT(struct entity *from, struct user *user, char *accname);
void hADMIN(struct entity *from, struct server *server);
void hASLL(struct entity *from, char *smark, struct server *target);
void hAWAY(struct entity *from, char *reason);
void hBURST(struct entity *from, struct channel *chan, time_t *ts);
void hCLEARMODE(struct entity *from, struct channel *chan, char *modes);
void hCONNECT(struct entity *from, char *servername, int *port, struct server *server);
void hCREATE(struct entity *from, char *channels, time_t *ts);
void hDESTRUCT(struct entity *from, struct channel *chan, time_t *ts);
void hDESYNCH(struct entity *from, char *msg);
void hEND_OF_BURST(struct entity *from);
void hEOB_ACK(struct entity *from);
void hERROR(struct entity *from);
void hGLINE1(struct entity *from, char *mask, time_t *duration, char *reason);
void hGLINE2(struct entity *from, char *mask, time_t *duration, time_t *lastmod, char *reason);
void hINFO(struct entity *from, struct server *target);
void hINVITE(struct entity *from, struct user *target, struct channel *chan);
void hJOIN(struct entity *from, struct user *user, struct channel *chan);
void hJUPE(struct entity *from, struct server *target, char *server, time_t *duration, time_t *lastmod, char *reason);
void hKICK(struct entity *from, struct channel *chan, struct user *target, char *reason);
void hKILL(struct entity *from, struct user *target, char *reason);
void hLINKS(struct entity *from, struct server *target, char *servermask);
void hLUSERS(struct entity *from, struct server *target);
void hMODE1(struct entity *from, struct user *user, struct manyargs *modechange);
void hMODE2(struct entity *from, struct channel *chan, struct manyargs *modechange, time_t *ts);
void hMOTD(struct entity *from, struct server *target);
void hNAMES(struct entity *from, struct server *server);
void hNICK1(struct entity *from, char *newnick, time_t *ts);
void hNICK2(struct entity *from, char *nick, int *hops, time_t *ts, char *ident, char *host, struct manyargs *mode, char *ip, char *unum, char *realname);
void hNOTICE(struct entity *from, char *target, char *msg);
void hOPMODE(struct entity *from, struct channel *chan, struct manyargs *mode, time_t *ts);
void hPART(struct entity *from, char *channels, char *reason);
void hPASS(struct entity *from, char *pass);
void hPING(struct entity *from, char *source, char *param);
void hPONG(struct entity *from, struct server *source, char *param);
void hPRIVMSG(struct entity *from, char *target, char *msg);
void hQUIT(struct entity *from, char *reason);
void hRPING1(struct entity *from, char *smask, struct server *startserver, char *text);
void hRPING2(struct entity *from, struct server *target, struct user *oper, time_t *start, long *startms);
void hRPONG1(struct entity *from, char *startserver, struct user *oper, time_t *start, long *startms);
void hRPONG2(struct entity *from, struct user *target, char *servername, long *ms, char *text);
void hSERVER(struct entity *from, char *name, int *hops, time_t *boot, time_t *link, char *protocol, char *numericstr, char *flags, char *descr);
void hSETHOST(struct entity *from, struct user *target, char *ident, char *host);
void hSETTIME(struct entity *from, time_t *ts, struct server *server);
void hSILENCE(struct entity *from, char *target, char *mask);
void hSQUIT(struct entity *from, char *target, time_t *link, char *reason);
void hSTATS(struct entity *from, char *stats, struct server *server, char *params);
void hTOPIC(struct entity *from, struct channel *chan, time_t *creation, time_t *change, char *topic);
void hTRACE(struct entity *from, char *param, struct server *target);
void hUPING(struct entity *from, char *smask, int *port, struct server *target, int *count);
void hVERSION(struct entity *from, struct server *target);
void hWALLCHOPS(struct entity *from, struct channel *chan, char *msg);
void hWALLOPS(struct entity *from, char *msg);
void hWALLUSERS(struct entity *from, char *msg);
void hWALLVOICES(struct entity *from, struct channel *chan, char *msg);
void hWHOIS(struct entity *from, struct server *server, char *search);

#endif /* HANDLE_H_ */

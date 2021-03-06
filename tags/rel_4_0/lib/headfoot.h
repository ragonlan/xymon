/*----------------------------------------------------------------------------*/
/* Hobbit monitor library.                                                    */
/*                                                                            */
/* Copyright (C) 2002-2005 Henrik Storner <henrik@storner.dk>                 */
/*                                                                            */
/* This program is released under the GNU General Public License (GPL),       */
/* version 2. See the file "COPYING" for details.                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef __HEADFOOT_H__
#define __HEADFOOT_H__

extern int unpatched_bbd;

extern void sethostenv(char *host, char *ip, char *svc, char *color);
extern void sethostenv_report(time_t reportstart, time_t reportend, double repwarn, double reppanic);
extern void sethostenv_snapshot(time_t snapshot);
extern void sethostenv_histlog(char *histtime);
extern void sethostenv_template(char *dir);
extern void sethostenv_refresh(int n);
extern void output_parsed(FILE *output, char *templatedata, int bgcolor, char *pagetype);
extern void headfoot(FILE *output, char *pagetype, char *pagepath, char *head_or_foot, int bgcolor);

#endif


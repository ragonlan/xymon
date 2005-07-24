/*----------------------------------------------------------------------------*/
/* Hobbit monitor library.                                                    */
/*                                                                            */
/* Copyright (C) 2002-2005 Henrik Storner <henrik@storner.dk>                 */
/*                                                                            */
/* This program is released under the GNU General Public License (GPL),       */
/* version 2. See the file "COPYING" for details.                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef __HOBBITRRD_H__
#define __HOBBITRRD_H__

/* This is for mapping a service -> an RRD file */
typedef struct {
   char *bbsvcname;
   char *hobbitrrdname;
} hobbitrrd_t;

/* This is for displaying an RRD file. */
typedef struct {
   char *hobbitrrdname;
   char *hobbitpartname;
   int  maxgraphs;
} hobbitgraph_t;

extern hobbitrrd_t *hobbitrrds;
extern hobbitgraph_t *hobbitgraphs;

extern hobbitrrd_t *find_hobbit_rrd(char *service, char *flags);
extern hobbitgraph_t *find_hobbit_graph(char *rrdname);
extern char *hobbit_graph_data(char *hostname, char *dispname, char *service, hobbitgraph_t *graphdef, int itemcount, int wantmeta);

#endif


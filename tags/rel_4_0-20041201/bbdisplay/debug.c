/*----------------------------------------------------------------------------*/
/* Big Brother webpage generator tool.                                        */
/*                                                                            */
/* This is a replacement for the "mkbb.sh" and "mkbb2.sh" scripts from the    */
/* "Big Brother" monitoring tool from BB4 Technologies.                       */
/*                                                                            */
/* Primary reason for doing this: Shell scripts perform badly, and with a     */
/* medium-sized installation (~150 hosts) it takes several minutes to         */
/* generate the webpages. This is a problem, when the pages are used for      */
/* 24x7 monitoring of the system status.                                      */
/*                                                                            */
/* Copyright (C) 2002 Henrik Storner <henrik@storner.dk>                      */
/*                                                                            */
/* This program is released under the GNU General Public License (GPL),       */
/* version 2. See the file "COPYING" for details.                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/

static char rcsid[] = "$Id: debug.c,v 1.32 2004-10-30 15:43:04 henrik Exp $";

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/time.h>

#include "bbgen.h"

void dumplinks(link_t *head)
{
	link_t *l;

	for (l = head; l; l = l->next) {
		printf("Link for host %s, URL/filename %s/%s\n", l->name, l->urlprefix, l->filename);
	}
}


void dumphosts(host_t *head, char *prefix)
{
	host_t *h;
	entry_t *e;
	char	format[512];

	strcpy(format, prefix);
	strcat(format, "Host: %s, ip: %s, name: %s, color: %d, old: %d, anywaps: %d, wapcolor: %d, pretitle: '%s', noprop-y: %s, noprop-r: %s, noprop-p: %s, noprop-ack: %s, link: %s, graphs: %s, waps: %s\n");

	for (h = head; (h); h = h->next) {
		printf(format, h->hostname, h->ip, textornull(h->displayname), h->color, h->oldage,
			h->anywaps, h->wapcolor,
			textornull(h->pretitle),
			textornull(h->nopropyellowtests), 
			textornull(h->nopropredtests), 
			textornull(h->noproppurpletests), 
			textornull(h->nopropacktests), 
			h->link->filename,
			textornull(h->larrdgraphs), textornull(h->waps));
		for (e = h->entries; (e); e = e->next) {
			printf("\t\t\t\t\tTest: %s, alert %d, propagate %d, state %d, age: %s, oldage: %d\n", 
				e->column->name, e->alert, e->propagate, e->color, e->age, e->oldage);
		}
	}
}

void dumpgroups(group_t *head, char *prefix, char *hostprefix)
{
	group_t *g;
	char    format[512];

	strcpy(format, prefix);
	strcat(format, "Group: %s, pretitle: '%s'\n");

	for (g = head; (g); g = g->next) {
		printf(format, textornull(g->title), textornull(g->pretitle));
		dumphosts(g->hosts, hostprefix);
	}
}

void dumphostlist(hostlist_t *head)
{
	hostlist_t *h;

	for (h=head; (h); h=h->next) {
		printf("Hostlist entry: Hostname %s\n", h->hostentry->hostname);
	}
}


void dumpstatelist(state_t *head)
{
	state_t *s;

	for (s=head; (s); s=s->next) {
		printf("test:%s, state: %d, alert: %d, propagate: %d, oldage: %d, age: %s\n",
			s->entry->column->name,
			s->entry->color,
			s->entry->alert,
			s->entry->propagate,
			s->entry->oldage,
			s->entry->age);
	}
}

void dumponepagewithsubs(bbgen_page_t *curpage, char *indent)
{
	bbgen_page_t *levelpage;

	char newindent[100];
	char newindentextra[105];

	strcpy(newindent, indent);
	strcat(newindent, "\t");
	strcpy(newindentextra, newindent);
	strcat(newindentextra, "    ");

	for (levelpage = curpage; (levelpage); levelpage = levelpage->next) {
		printf("%sPage: %s, color=%d, oldage=%d, title=%s, pretitle=%s\n", 
			indent, levelpage->name, levelpage->color, levelpage->oldage, textornull(levelpage->title), textornull(levelpage->pretitle));

		dumpgroups(levelpage->groups, newindent, newindentextra);
		dumphosts(levelpage->hosts, newindentextra);
		dumponepagewithsubs(levelpage->subpages, newindent);
	}
}

void dumpall(bbgen_page_t *head)
{
	dumponepagewithsubs(head, "");
}



/*----------------------------------------------------------------------------*/
/* Hobbit monitor.                                                            */
/*                                                                            */
/* This is a library module for Hobbit, responsible for loading the host-,    */
/* page-, and column-links defined in the BB directory structure.             */
/*                                                                            */
/* Copyright (C) 2004 Henrik Storner <henrik@hswn.dk>                         */
/*                                                                            */
/* This program is released under the GNU General Public License (GPL),       */
/* version 2. See the file "COPYING" for details.                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/

static char rcsid[] = "$Id: links.c,v 1.7 2005-01-20 22:02:23 henrik Exp $";

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>

#include "libbbgen.h"

/* Info-link definitions. */
typedef struct link_t {
	char	*name;
	char	*filename;
	char	*urlprefix;	/* "/help", "/notes" etc. */
	struct link_t	*next;
} link_t;

static link_t *linkhead = NULL;
static char *notesskin = NULL;
static char *helpskin = NULL;
static char *columndocurl = NULL;

static link_t *init_link(char *filename, char *urlprefix)
{
	char *p;
	link_t *newlink = NULL;

	dprintf("init_link(%s, %s)\n", textornull(filename), textornull(urlprefix));

	newlink = (link_t *) malloc(sizeof(link_t));
	newlink->filename = strdup(filename);
	newlink->urlprefix = urlprefix;
	newlink->next = NULL;

	p = strrchr(filename, '.');
	if (p == NULL) p = (filename + strlen(filename));

	if ( (strcmp(p, ".php") == 0)    ||
             (strcmp(p, ".php3") == 0)   ||
             (strcmp(p, ".asp") == 0)    ||
             (strcmp(p, ".doc") == 0)    ||
	     (strcmp(p, ".shtml") == 0)  ||
	     (strcmp(p, ".phtml") == 0)  ||
	     (strcmp(p, ".html") == 0)   ||
	     (strcmp(p, ".htm") == 0))      
	{
		*p = '\0';
	}

	/* Without extension, this time */
	newlink->name = strdup(filename);

	return newlink;
}

static link_t *load_links(const char *directory, char *urlprefix)
{
	DIR		*bblinks;
	struct dirent 	*d;
	char		fn[PATH_MAX];
	link_t		*curlink, *toplink, *newlink;

	dprintf("load_links(%s, %s)\n", textornull(directory), textornull(urlprefix));

	toplink = curlink = NULL;
	bblinks = opendir(directory);
	if (!bblinks) {
		errprintf("Cannot read links in directory %s\n", directory);
		return NULL;
	}

	MEMDEFINE(fn);

	while ((d = readdir(bblinks))) {
		strcpy(fn, d->d_name);
		newlink = init_link(fn, urlprefix);
		if (newlink) {
			if (toplink == NULL) {
				toplink = newlink;
			}
			else {
				curlink->next = newlink;
			}
			curlink = newlink;
		}
	}
	closedir(bblinks);

	MEMUNDEFINE(fn);

	return toplink;
}

void load_all_links(void)
{
	link_t *l, *head1, *head2;
	char dirname[PATH_MAX];
	char *p;

	MEMDEFINE(dirname);

	dprintf("load_all_links()\n");

	if (notesskin) { xfree(notesskin); notesskin = NULL; }
	if (helpskin) { xfree(helpskin); helpskin = NULL; }
	if (columndocurl) { xfree(columndocurl); columndocurl = NULL; }

	if (xgetenv("BBNOTESSKIN")) notesskin = strdup(xgetenv("BBNOTESSKIN"));
	else { 
		notesskin = (char *) malloc(strlen(xgetenv("BBWEB")) + strlen("/notes") + 1);
		sprintf(notesskin, "%s/notes", xgetenv("BBWEB"));
	}

	if (xgetenv("BBHELPSKIN")) helpskin = strdup(xgetenv("BBHELPSKIN"));
	else { 
		helpskin = (char *) malloc(strlen(xgetenv("BBWEB")) + strlen("/help") + 1);
		sprintf(helpskin, "%s/help", xgetenv("BBWEB"));
	}

	if (xgetenv("COLUMNDOCURL")) columndocurl = strdup(xgetenv("COLUMNDOCURL"));

	strcpy(dirname, xgetenv("BBNOTES"));
	head1 = load_links(dirname, notesskin);

	/* Change xxx/xxx/xxx/notes into xxx/xxx/xxx/help */
	p = strrchr(dirname, '/'); *p = '\0'; strcat(dirname, "/help");
	head2 = load_links(dirname, helpskin);

	if (head1) {
		/* Append help-links to list of notes-links */
		for (l = head1; (l->next); l = l->next) ;
		l->next = head2;
	}
	else {
		/* /notes was empty, so just return the /help list */
		head1 = head2;
	}

	linkhead = head1;

	MEMUNDEFINE(dirname);
}


static link_t *find_link(const char *name)
{
	/* We cache the last link searched for */
	static link_t *lastlink = NULL;
	link_t *l;

	if (lastlink && (strcmp(lastlink->name, name) == 0))
		return lastlink;

	for (l=linkhead; (l && (strcmp(l->name, name) != 0)); l = l->next);
	lastlink = l;

	return (l ? l : NULL);
}


char *columnlink(char *colname)
{
	static char *linkurl = NULL;
	link_t *link = find_link(colname);

	if (linkurl == NULL) linkurl = (char *)malloc(PATH_MAX);

	if (columndocurl) {
		sprintf(linkurl, columndocurl, colname);
	}
	else if (link) {
		sprintf(linkurl, "%s/%s", link->urlprefix, link->filename);
	}
	else {
		sprintf(linkurl, "%s/help/bb-help.html#%s", xgetenv("BBWEB"), colname);
	}
	
	return linkurl;
}


char *hostlink(char *hostname)
{
	static char *linkurl = NULL;
	link_t *link = find_link(hostname);

	if (linkurl == NULL) linkurl = (char *)malloc(PATH_MAX);

	if (link) {
		sprintf(linkurl, "%s/%s", link->urlprefix, link->filename);
		return linkurl;
	}

	return NULL;
}


/*----------------------------------------------------------------------------*/
/* Big Brother webpage generator tool.                                        */
/*                                                                            */
/* This is a replacement for the "bb-hist.sh" script                          */
/*                                                                            */
/* Primary reason for doing this: Shell scripts perform badly, and with a     */
/* medium-sized installation (~150 hosts) it takes several minutes to         */
/* generate the webpages. This is a problem, when the pages are used for      */
/* 24x7 monitoring of the system status.                                      */
/*                                                                            */
/* Copyright (C) 2003 Henrik Storner <henrik@storner.dk>                      */
/*                                                                            */
/* This program is released under the GNU General Public License (GPL),       */
/* version 2. See the file "COPYING" for details.                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/

static char rcsid[] = "$Id: bb-hist.c,v 1.5 2003-06-24 05:30:10 henrik Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bbgen.h"
#include "util.h"
#include "reportdata.h"
#include "debug.h"

#define PIXELS 960	/* 864 ? */
#define FACTOR 90	/* 86400 / PIXELS */

void generate_history(FILE *htmlrep, char *hostname, char *service, char *ip, int entrycount,
		      reportinfo_t *repinfo, replog_t *log24hours, replog_t *loghead)
{
	char *bgcols[2] = { "\"#000000\"", "\"#000033\"" };
	int curbg = 0;
	time_t yesterday, today;
	int pctfirst, pctlast, pctsum;
	struct tm *tmbuf;
	time_t secs;
	int starthour, hour;
	replog_t *colorlog, *walk, *tmp;

	today = time(NULL); yesterday = today-86400;

	/* Compute the percentage of the first (incomplete) hour */
	tmbuf = localtime(&yesterday);
	secs = 3600 - (tmbuf->tm_min*60 + tmbuf->tm_sec);
	pctfirst = secs / FACTOR;

	/* Compute the percentage of the last (incomplete) hour */
	tmbuf = localtime(&today);
	starthour = tmbuf->tm_hour;
	secs = tmbuf->tm_min*60 + tmbuf->tm_sec;
	pctlast = secs / FACTOR;

	sethostenv(hostname, ip, service, colorname(COL_GREEN));

	headfoot(htmlrep, "hist", "", "header", COL_GREEN);

	fprintf(htmlrep, "\n");

	fprintf(htmlrep, "<CENTER>\n");
	fprintf(htmlrep, "<BR>\n");

	/* Create the color-bar */
	fprintf(htmlrep, "<TABLE WIDTH=\"%d\" BORDER=0 BGCOLOR=\"#666666\">\n", PIXELS);
	fprintf(htmlrep, "<TR><TD ALIGN=CENTER>\n");

	/* The date stamps */
	fprintf(htmlrep, "<TABLE WIDTH=\"100%%\" BORDER=1 BGCOLOR=\"#000033\">\n");
	fprintf(htmlrep, "<TR>\n");
	fprintf(htmlrep, "<TD WIDTH=\"50%%\" ALIGN=LEFT><B>%s</B></TD>\n", ctime(&yesterday));
	fprintf(htmlrep, "<TD WIDTH=\"50%%\" ALIGN=RIGHT><B>%s</B></TD>\n", ctime(&today));
	fprintf(htmlrep, "</TR>\n");
	fprintf(htmlrep, "</TABLE>\n");

	/* The hour line */
	pctsum = pctfirst + pctlast;
	fprintf(htmlrep, "<TABLE WIDTH=\"100%%\" BORDER=0 BGCOLOR=\"#000033\">\n");
	fprintf(htmlrep, "<TR>\n");
	fprintf(htmlrep, "<TD WIDTH=%d ALIGN=LEFT><B>&nbsp;</B></TD>\n", pctfirst);
	for (hour = ((starthour + 1) % 24); (hour != starthour); hour = ((hour + 1) % 24)) {
		fprintf(htmlrep, "<TD WIDTH=%d ALIGN=LEFT><B>%d</B></TD>\n", (PIXELS / 24), hour);
		pctsum += (PIXELS / 24);
	}
	fprintf(htmlrep, "<TD WIDTH=%d ALIGN=LEFT><B>%d</B></TD>\n", pctlast, starthour);
	fprintf(htmlrep, "<!-- pctsum = %d -->\n", pctsum);
	fprintf(htmlrep, "</TR>\n");
	fprintf(htmlrep, "</TABLE>\n");

	/* The actual color bar */
	fprintf(htmlrep, "<TABLE WIDTH=\"100%%\" BORDER=0 BGCOLOR=\"#000033\">\n");
	fprintf(htmlrep, "<TR>\n");
	fprintf(htmlrep, "<FONT SIZE=1>\n");

	/* Need to re-sort the 24-hour log to chronological order */
	colorlog = NULL; pctsum = 0;
	for (walk = log24hours; (walk); walk = tmp) {
		tmp = walk->next;
		walk->next = colorlog;
		colorlog = walk;
		walk = tmp;
	}
	for (walk = colorlog; (walk); walk = walk->next) {
		int pct = (walk->duration / FACTOR);

		pctsum += pct;
		fprintf(htmlrep, "<TD WIDTH=%d BGCOLOR=%s NOWRAP>&nbsp</TD>\n", 
			pct, colorname(walk->color));
	}

	fprintf(htmlrep, "<!-- pctsum = %d -->\n", pctsum);
	fprintf(htmlrep, "</FONT></TR>\n");
	fprintf(htmlrep, "</TABLE>\n");

	fprintf(htmlrep, "</TD></TR></TABLE>\n");


	fprintf(htmlrep, "<CENTER>\n");
	fprintf(htmlrep, "<BR><FONT %s><H2>%s - %s</H2></FONT>\n", getenv("MKBBROWFONT"), hostname, service);
	fprintf(htmlrep, "<TABLE BORDER=0 BGCOLOR=\"#333333\" CELLPADDING=3>\n");
	fprintf(htmlrep, "<TR BGCOLOR=\"#000000\">\n");
	fprintf(htmlrep, "</TR>\n");
	fprintf(htmlrep, "<TR></TR>\n");
	fprintf(htmlrep, "<TR><TD COLSPAN=6 ALIGN=CENTER><B>Last 24 hours</B></TD></TR>\n");
	fprintf(htmlrep, "<TR BGCOLOR=\"#000000\">\n");
	fprintf(htmlrep, "<TD ALIGN=CENTER><IMG SRC=\"%s/%s\" ALT=\"%s\" HEIGHT=%s WIDTH=%s BORDER=0></TD>\n", 
		getenv("BBSKIN"), dotgiffilename(COL_GREEN, 0, 1), colorname(COL_GREEN), getenv("DOTHEIGHT"), getenv("DOTWIDTH"));
	fprintf(htmlrep, "<TD ALIGN=CENTER><IMG SRC=\"%s/%s\" ALT=\"%s\" HEIGHT=%s WIDTH=%s BORDER=0></TD>\n", 
		getenv("BBSKIN"), dotgiffilename(COL_YELLOW, 0, 1), colorname(COL_YELLOW), getenv("DOTHEIGHT"), getenv("DOTWIDTH"));
	fprintf(htmlrep, "<TD ALIGN=CENTER><IMG SRC=\"%s/%s\" ALT=\"%s\" HEIGHT=%s WIDTH=%s BORDER=0></TD>\n", 
		getenv("BBSKIN"), dotgiffilename(COL_RED, 0, 1), colorname(COL_RED), getenv("DOTHEIGHT"), getenv("DOTWIDTH"));
	fprintf(htmlrep, "<TD ALIGN=CENTER><IMG SRC=\"%s/%s\" ALT=\"%s\" HEIGHT=%s WIDTH=%s BORDER=0></TD>\n", 
		getenv("BBSKIN"), dotgiffilename(COL_PURPLE, 0, 1), colorname(COL_PURPLE), getenv("DOTHEIGHT"), getenv("DOTWIDTH"));
	fprintf(htmlrep, "<TD ALIGN=CENTER><IMG SRC=\"%s/%s\" ALT=\"%s\" HEIGHT=%s WIDTH=%s BORDER=0></TD>\n", 
		getenv("BBSKIN"), dotgiffilename(COL_CLEAR, 0, 1), colorname(COL_CLEAR), getenv("DOTHEIGHT"), getenv("DOTWIDTH"));
	fprintf(htmlrep, "<TD ALIGN=CENTER><IMG SRC=\"%s/%s\" ALT=\"%s\" HEIGHT=%s WIDTH=%s BORDER=0></TD>\n", 
		getenv("BBSKIN"), dotgiffilename(COL_BLUE, 0, 1), colorname(COL_BLUE), getenv("DOTHEIGHT"), getenv("DOTWIDTH"));
	fprintf(htmlrep, "</TR>\n");
	fprintf(htmlrep, "<TR BGCOLOR=\"#000033\">\n");
	fprintf(htmlrep, "<TD ALIGN=CENTER><B>%.2f%%</B></TD>\n", repinfo->pct[COL_GREEN]);
	fprintf(htmlrep, "<TD ALIGN=CENTER><B>%.2f%%</B></TD>\n", repinfo->pct[COL_YELLOW]);
	fprintf(htmlrep, "<TD ALIGN=CENTER><B>%.2f%%</B></TD>\n", repinfo->pct[COL_RED]);
	fprintf(htmlrep, "<TD ALIGN=CENTER><B>%.2f%%</B></TD>\n", repinfo->pct[COL_PURPLE]);
	fprintf(htmlrep, "<TD ALIGN=CENTER><B>%.2f%%</B></TD>\n", repinfo->pct[COL_CLEAR]);
	fprintf(htmlrep, "<TD ALIGN=CENTER><B>%.2f%%</B></TD>\n", repinfo->pct[COL_BLUE]);
	fprintf(htmlrep, "</TR>\n");
	fprintf(htmlrep, "<TR BGCOLOR=\"#000000\">\n");
	fprintf(htmlrep, "<TD COLSPAN=6 ALIGN=CENTER>\n");
	fprintf(htmlrep, "<FONT %s><B>[Total may not equal 100%%]</B></TD> </TR>\n", getenv("MKBBCOLFONT"));
	fprintf(htmlrep, "</TABLE>\n");
	fprintf(htmlrep, "</CENTER>\n");


	fprintf(htmlrep, "<BR><BR>\n");


	fprintf(htmlrep, "<CENTER>\n");
	fprintf(htmlrep, "<TABLE BORDER=0 BGCOLOR=\"#333333\" CELLSPACING=3>\n");
	fprintf(htmlrep, "<TR>\n");
	if (entrycount) {
		fprintf(htmlrep, "<TD COLSPAN=3 ALIGN=CENTER><B>Last %d log entries</B> ", entrycount);
		fprintf(htmlrep, "<A HREF=\"%s/bb-hist.sh?HISTFILE=%s.%s&ENTRIES=all\">(Full HTML log)</A></TD>\n", 
			getenv("CGIBINURL"), commafy(hostname), service);
	}
	else {
		fprintf(htmlrep, "<TD COLSPAN=3 ALIGN=CENTER><B>All log entries</B></TD>\n");
	}
	fprintf(htmlrep, "</TR>\n");
	fprintf(htmlrep, "<TR BGCOLOR=\"#333333\">\n");
	fprintf(htmlrep, "<TD ALIGN=CENTER><FONT %s><B>Date</B></TD>\n", getenv("MKBBCOLFONT"));
	fprintf(htmlrep, "<TD ALIGN=CENTER><FONT %s><B>Status</B></TD>\n", getenv("MKBBCOLFONT"));
	fprintf(htmlrep, "<TD ALIGN=CENTER><FONT %s><B>Duration</B></TD>\n", getenv("MKBBCOLFONT"));
	fprintf(htmlrep, "</TR>\n");

	for (walk = loghead; (walk); walk = walk->next) {
		char start[30];

		strftime(start, sizeof(start), "%a %b %d %H:%M:%S %Y", localtime(&walk->starttime));

		fprintf(htmlrep, "<TR BGCOLOR=%s>\n", bgcols[curbg]); curbg = (1-curbg);
		fprintf(htmlrep, "<TD ALIGN=LEFT NOWRAP>%s</TD>\n", start);
		fprintf(htmlrep, "<TD ALIGN=CENTER BGCOLOR=\"#000000\">");
		fprintf(htmlrep, "<A HREF=\"%s/bb-histlog.sh?HOST=%s&SERVICE=%s&TIMEBUF=%s\">", 
			getenv("CGIBINURL"), hostname, service, walk->timespec);
		fprintf(htmlrep, "<IMG SRC=\"%s/%s\" ALT=\"%s\" HEIGHT=%s WIDTH=%s BORDER=0>", 
			getenv("BBSKIN"), dotgiffilename(walk->color, 0, 1), colorname(walk->color),
			getenv("DOTHEIGHT"), getenv("DOTWIDTH"));
		fprintf(htmlrep, "</TD>\n");

		fprintf(htmlrep, "<TD ALIGN=CENTER>%s</TD>\n", durationstr(walk->duration));
		fprintf(htmlrep, "</TR>\n\n");
	}


	fprintf(htmlrep, "</TABLE>\n");


	fprintf(htmlrep, "<BR><BR>\n");

	/* BBHISTEXT extensions */
	do_bbext(htmlrep, "BBHISTEXT", "hist");

	fprintf(htmlrep, "</CENTER>\n");

	headfoot(htmlrep, "histlog", "", "footer", COL_GREEN);
}


/*
 * This program is invoked via CGI with QUERY_STRING containing:
 *
 *	HISTFILE=www,sample,com.conn
 *	ENTRIES=50
 */

/* These are dummy vars needed by stuff in util.c */
hostlist_t      *hosthead = NULL;
link_t          *linkhead = NULL;
link_t  null_link = { "", "", "", NULL };

/* These are needed, but not actually used */
double reportgreenlevel = 99.995;
double reportwarnlevel = 98.0;

char *hostname = "";
char *service = "";
char *ip = "";
int entrycount = 50;

char *reqenv[] = {
"BBHIST",
"BBHISTLOGS",
"BBREP",
"BBREPURL",
"BBSKIN",
"CGIBINURL",
"DOTWIDTH",
"DOTHEIGHT",
"MKBBCOLFONT",
"MKBBROWFONT",
NULL };

static void errormsg(char *msg)
{
	printf("Content-type: text/html\n\n");
	printf("<html><head><title>Invalid request</title></head>\n");
	printf("<body>%s</body></html>\n", msg);
	exit(1);
}

static void parse_query(void)
{
	char *query, *token;

	if (getenv("QUERY_STRING") == NULL) {
		errormsg("Invalid request");
		return;
	}
	else query = urldecode("QUERY_STRING");

	token = strtok(query, "&");
	while (token) {
		char *val;
		
		val = strchr(token, '='); if (val) { *val = '\0'; val++; }

		if (argnmatch(token, "HISTFILE")) {
			char *p = strrchr(val, '.');

			if (p) { *p = '\0'; service = malcop(p+1); }
			hostname = malcop(val);
			while ((p = strchr(hostname, ','))) *p = '.';
		}
		else if (argnmatch(token, "IP")) {
			ip = malcop(val);
		}
		else if (argnmatch(token, "ENTRIES")) {
			if (strcmp(val, "all") == 0) entrycount = 0;
			else entrycount = atoi(val);
		}

		token = strtok(NULL, "&");
	}

	free(query);
}


int main(int argc, char *argv[])
{
	char histlogfn[MAX_PATH];
	char tailcmd[MAX_PATH];
	FILE *fd;
	reportinfo_t repinfo, dummyrep;
	time_t now;
	replog_t *log24hours;

	envcheck(reqenv);
	parse_query();

	sprintf(histlogfn, "%s/%s.%s", getenv("BBHIST"), commafy(hostname), service);
	fd = fopen(histlogfn, "r");
	if (fd == NULL) {
		errormsg("Cannot open history file");
	}
	now = time(NULL);

	parse_historyfile(fd, &repinfo, NULL, NULL, now-86400, now, 1);
	log24hours = save_replogs();

	if (entrycount == 0) {
		/* All entries - just rewind the history file and do all of them */
		rewind(fd);
		parse_historyfile(fd, &dummyrep, NULL, NULL, 0, now, 1);
		fclose(fd);
	}
	else {
		/* Last 50 entries - we cheat and use "tail" in a pipe to pick the entries */
		fclose(fd);
		sprintf(tailcmd, "tail -%d %s", entrycount, histlogfn);
		fd = popen(tailcmd, "r");
		if (fd == NULL) errormsg("Cannot run tail on the histfile");
		parse_historyfile(fd, &dummyrep, NULL, NULL, 0, now, 1);
		pclose(fd);
	}


	/* Now generate the webpage */
	printf("Content-Type: text/html\n\n");

	generate_history(stdout, hostname, service, ip, entrycount, &repinfo, log24hours, reploghead);

	return 0;
}


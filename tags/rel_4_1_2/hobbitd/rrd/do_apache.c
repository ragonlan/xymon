/*----------------------------------------------------------------------------*/
/* Hobbit RRD handler module.                                                 */
/*                                                                            */
/* Copyright (C) 2004-2005 Henrik Storner <henrik@hswn.dk>                    */
/*                                                                            */
/* This program is released under the GNU General Public License (GPL),       */
/* version 2. See the file "COPYING" for details.                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/

static char apache_rcsid[] = "$Id: do_apache.c,v 1.8 2005-06-05 09:24:39 henrik Exp $";

int do_apache_rrd(char *hostname, char *testname, char *msg, time_t tstamp)
{
	static char *apache_params[] = { "rrdcreate", rrdfn, 
					 "DS:TA:DERIVE:600:0:U",
					 "DS:TKB:DERIVE:600:0:U",
					 "DS:BW:GAUGE:600:1:U",
					 "DS:IW:GAUGE:600:1:U",
					 "DS:CPU:GAUGE:600:0:U",
					 "DS:REQPERSEC:GAUGE:600:0:U",
					 rra1, rra2, rra3, rra4, NULL };
	static char *apache_tpl      = NULL;

	char *markers[] = { "Total Accesses:", "Total kBytes:", 
			    "BusyWorkers:", "IdleWorkers:", "CPULoad:", "ReqPerSec:", NULL };
	int i;
	char *p, *eoln;

	if (apache_tpl == NULL) apache_tpl = setup_template(apache_params);

	/* Apache 1.x uses BusyServers/IdleServers. Convert the status to Apache 2.0 format */
	if ((p = strstr(msg, "BusyServers:")) != NULL) memcpy(p, "BusyWorkers:", strlen("BusyWorkers:"));
	if ((p = strstr(msg, "IdleServers:")) != NULL) memcpy(p, "IdleWorkers:", strlen("IdleWorkers:"));

	strcpy(rrdfn, "apache.rrd");
	sprintf(rrdvalues, "%d", (int)tstamp);
	i = 0;
	while (markers[i]) {
		strcat(rrdvalues, ":"); 
		p = strstr(msg, markers[i]);
		if (p) {
			eoln = strchr(p, '\n');
			if (eoln) *eoln = '\0';
			p = strchr(p, ':')+1;
			p += strspn(p, " ");
			strcat(rrdvalues, p);
			if (eoln) *eoln = '\n';
		}
		else {
			strcat(rrdvalues, "U");
		}
		i++;
	}

	return create_and_update_rrd(hostname, rrdfn, apache_params, apache_tpl);
}


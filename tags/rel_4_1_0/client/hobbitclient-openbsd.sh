#!/bin/sh
#----------------------------------------------------------------------------#
# OpenBSD client for Hobbit                                                  #
#                                                                            #
# Copyright (C) 2005 Henrik Storner <henrik@hswn.dk>                         #
#                                                                            #
# This program is released under the GNU General Public License (GPL),       #
# version 2. See the file "COPYING" for details.                             #
#                                                                            #
#----------------------------------------------------------------------------#
#
# $Id: hobbitclient-openbsd.sh,v 1.3 2005-07-24 11:32:51 henrik Exp $

echo "[date]"
date
echo "[uname]"
uname -a
echo "[uptime]"
uptime
echo "[who]"
who
echo "[df]"
df -P -tnonfs,kernfs,procfs,cd9660
echo "[meminfo]"
$BBHOME/bin/openbsd-meminfo
echo "[swapctl]"
/sbin/swapctl -s
echo "[netstat]"
netstat -s
echo "[ps]"
ps -axw
echo "[top]"
top -n 20
# vmstat
nohup sh -c "vmstat 300 2 1>$BBTMP/hobbit_vmstat.$$ 2>&1; mv $BBTMP/hobbit_vmstat.$$ $BBTMP/hobbit_vmstat" </dev/null >/dev/null 2>&1 &
sleep 5
if test -f $BBTMP/hobbit_vmstat; then echo "[vmstat]"; cat $BBTMP/hobbit_vmstat; rm -f $BBTMP/hobbit_vmstat; fi

exit


#
# Regular cron jobs for the mp3playerwifi package.
#
0 4	* * *	root	[ -x /usr/bin/mp3playerwifi_maintenance ] && /usr/bin/mp3playerwifi_maintenance

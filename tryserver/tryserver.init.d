#! /bin/sh
### BEGIN INIT INFO
# Provides:          tryserver
# Required-Start:    $syslog $remote_fs
# Required-Stop:     $syslog $remote_fs
# Should-Start:      $network avahi-daemon slapd nslcd
# Should-Stop:       $network
# Default-Start:     2 3 4 5
# Default-Stop:      1
# Short-Description: Tryserver server
# Description:       Manage the tryserver
### END INIT INFO

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin

case "$1" in
  start)
	/home/bin/tryserver >/dev/null 2>&1 &
	;;
  stop)
	killall tryserver
	;;
  restart)
	killall tryserver
	/home/bin/tryserver >/dev/null 2>&1 &
	;;
  status)
	status_of_proc -p "$PIDFILE" "$DAEMON" "$NAME" && exit 0 || exit $?
	;;
  *)
	echo "Usage: $SCRIPTNAME {start|stop|restart|status}" >&2
	exit 3
	;;
esac

exit 0


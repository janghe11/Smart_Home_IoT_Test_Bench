#!/bin/sh
### BEGIN INIT INFO
# Provides: avr_daemon
# chkconfig : 35 90 20
# Default-Start: S
# Short-Description:  Enable RS-232C polling from AVR.
# Description: Scan RS-232C polling realtime from ttyUSB0 when AVR send unsigned char data.
### END INIT INFO

export AVR_DAEMON_HOME=/home/pi/public_html

case "$1" in
  start)
    echo -n "Starting avr_daemon : "
    daemon $AVR_DAEMON_HOME/avr_daemon
    ;;
  stop)
    ;;
  restart)
    echo -n "Restart avr_daemon : "
    $0 stop
    $0 start
    ;;
    *)
  echo "Usage : $0 {start | stop | restart}"
  exit 1
esac
  exit 0

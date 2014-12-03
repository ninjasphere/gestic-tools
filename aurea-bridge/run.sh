#!/bin/bash

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root" 1>&2
	exit 1
fi

set -x

stop spheramid || true
stop ledcontroller || true
stop serial-console-gadget || true

# reconfigure g_serial
if lsmod |grep -q g_serial
then
  echo "shut down any progs that are not Aurea"
  read -p "press enter to continue" x
  rmmod -f g_serial
fi
modprobe g_serial idVendor=0x04D8 idProduct=0x000A

# reconfigure gestic
rmmod gestic
if ! lsmod |grep -q gestic
then
  echo load module gestic
  modprobe gestic bridge_spam_reads=1
fi

# bind them together. there will be lots of -EAGAIN "errors" as reads fail, that is safe to ignore.
socat -s -x gopen:/dev/gestic,nonblock file:/dev/ttyGS0,raw,echo=0,cs8,clocal,nonblock || true

# revert the driver options
rmmod g_serial
rmmod gestic
modprobe gestic
modprobe g_serial

echo all back to normal

sdk
===

This is a copy of the GestIC SDK v1.1 taken from Microchip and modified to use our kernel driver and to compile using GCC on Linux.
The primary change is the removal of the serial port `ioctl` calls since the `/dev/gestic` interface doesn't support them.

Original source: http://www.microchip.com/pagehandler/en-us/technology/gestic/home.html?tab=t2

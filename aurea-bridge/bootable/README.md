GestIC Aurea Studio Bridge - USB enabled for Ninja Sphere
======================

This directory contains scripts to build a USB flash drive that installs the required packages to have the Ninja Sphere act as a bridge for the GestIC chip, compatible with Aurea Studio and used for calibration.

The packages that are bundled are:
 * ```ninja-fix-applicator``` - to safely install packages at boot (ensures filesystem sync, etc)
 * ```gestic-usb-calibrator``` - a couple of init scripts that modify gadget serial boot behaviour
 * ```socat``` - a dependency for above

To prepare, simply run:
```
make
```

The resulting ```usb.zip``` can be distributed and extracted to a USB flash drive, and will install the appropriate packages when a Ninja Sphere is booted with it plugged in.

Installation
------------

These steps must be followed the first time

 * Extract the ZIP file to an empty USB flash drive formatted as FAT32.
 * Unmount the flash drive safely
 * Power off your Sphere
 * Insert the USB flash drive
 * Re-insert the power to the Sphere, and let it to boot
 * Wait until a red and white stop sign appears on the LED matrix
 * Unplug the power from the Sphere
 * Remove the USB flash drive
 * Make sure to remove the files from the USB flash drive so they are not installed a second time

Usage
-----

 * On your PC, delete all files from the USB flash drive.
 * Create a new file called ```gestic-serial-enable.txt```, containing anything (or even empty).
 * Unmount the flash drive safely
 * Insert the USB flash drive in the Sphere
 * Plug in a USB cable to the Sphere's mini-USB port, connecting the other end to a Windows PC running Aurea
 * Power on the Sphere
 * Instead of a normal boot, the LED matrix will flash a few times then turn off, and a Aurea-compatible USB device will appear to the host plugged in to the mini-USB port.
 * When you are done, turn off the Sphere and remove the USB. Future boots without the USB drive will proceed normally, with a USB serial console.

Issues
------

 * If you see a blue spinner or mobile phone icon for more than 10-20 seconds during installation, the USB package install did not success. Try adjusting the physical USB connection and reboot to try again.
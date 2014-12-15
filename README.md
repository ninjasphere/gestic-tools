gestic-tools
============

A collection of tools and libraries for use with the MGC3130 GestIC controller from Microchip as embedded in the Ninja Sphere.

The included modules are summarised below. Separate `LICENSE` files are included in each directory.

kernel-module
-------------

Provides a streaming character device at `/dev/gestic` which mimicks the USB-I2C bridge provided in the Microchip Hillstar 
development kit as closely as possible.

sdk
---

This is a copy of the GestIC SDK v1.1 taken from Microchip and modified to use our kernel driver and to compile using GCC on Linux.
	
aurea-bridge
------------

This small bash script re-appropriates the USB gadget serial driver (usually used for USB console access) to act as a USB-I2C bridge.

calibration
-----------
calibration files, lib version is in name, timestamp in name.
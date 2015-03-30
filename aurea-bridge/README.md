aurea-bridge
============

This small bash script re-appropriates the USB gadget serial driver (usually used for USB console access) to act as a USB-I2C bridge.

This is done by using socat to bind the drivers together. Currently this requires a special load flag for the gestic kernel module,
which causes a large number of harmless errors to appear while running socat as it constantly attemps to read even when no data is
available. This is a workaround that makes it more compatible with the hardware USB-I2C bridge provided by Microchip.

Start by installing dependencies:

```
sudo with-rw apt-get install socat
```

Then copy "run.sh" from this directory to your sphere.

To use, install the Aurea software for the MGC3130 from Microchip on a Windows computer, plug in your device via USB, then run:

```
sudo ./run.sh
```


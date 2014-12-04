ARCH=arm

KDIR=~/kernel-sources/3.12/VAR-SOM-AM33-SDK7-Kernel

BUILD_VARS=ARCH=$(ARCH) CROSS_COMPILE=/opt/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/bin/arm-linux-gnueabihf-

obj-m += gestic.o

all:
	$(BUILD_VARS) make -C $(KDIR) M=$(PWD) modules
	rm -rf .pkg
	$(BUILD_VARS) make -C $(KDIR) M=$(PWD) INSTALL_MOD_PATH=$(PWD)/.pkg modules_install

clean:
	$(BUILD_VARS) make -C $(KDIR) M=$(PWD) clean
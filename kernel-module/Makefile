ARCH=arm
#KDIR=~/variscite/linux-3.2.0-sdk06.00.00.00.var_r13
#KDIR=~/VAR-SOM-AM33-SDK7-Kernel/
#KDIR=/home/yocto/yocto_varsomam33/tisdk/build/arago-tmp-external-linaro-toolchain/work/varsomam33-oe-linux-gnueabi/linux-ti-variscite/3.12.10-r21k+gitrAUTOINC+da24dac5ed501c9c0c64e836c8aa188067db8194+ninja3/git
KDIR=~/yocto_varsomam33/tisdk/build/arago-tmp-external-linaro-toolchain/work/varsomam33-oe-linux-gnueabi/linux-ti-variscite/3.12.10-r21k+gitrAUTOINC+da24dac5ed501c9c0c64e836c8aa188067db8194+ninja3/git
#KDIR=/home/yocto/yocto_varsomam33/tisdk/build/arago-tmp-external-linaro-toolchain/work/varsomam33-oe-linux-gnueabi/linux-ti-variscite/3.12.10-r21k+gitrAUTOINC+da24dac5ed501c9c0c64e836c8aa188067db8194+ninja3/package/usr/src/kernel

#BUILD_VARS=ARCH=$(ARCH) CROSS_COMPILE=/usr/local/ti-sdk-am335x-evm/linux-devkit/sysroots/i686-arago-linux/usr/bin/arm-linux-gnueabihf-
BUILD_VARS=ARCH=$(ARCH) CROSS_COMPILE=/opt/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/bin/arm-linux-gnueabihf-

obj-m += gestic.o

all:
	$(BUILD_VARS) make -C $(KDIR) M=$(PWD) modules

clean:
	$(BUILD_VARS) make -C $(KDIR) M=$(PWD) clean

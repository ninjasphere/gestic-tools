all: socat_1.7.2.3-1_armhf.deb gestic-usb-calibrator_0.0.1_armhf.deb ninja-fix-applicator_0.1.0~trustyspheramid-8_armhf.deb
	rm -rf usb usb.zip
	mkdir -p usb
	cp socat_1.7.2.3-1_armhf.deb usb/
	cp gestic-usb-calibrator_0.0.1_armhf.deb usb/
	cp ninja-fix-applicator_0.1.0~trustyspheramid-8_armhf.deb usb/
	cd usb; shasum * > usb-packages.manifest
	cd usb; zip -r ../usb.zip .

socat_1.7.2.3-1_armhf.deb:
	wget http://s3.amazonaws.com/ninja-partialverse-repo/pool/main/s/socat/socat_1.7.2.3-1_armhf.deb

gestic-usb-calibrator_0.0.1_armhf.deb:
	fpm -s dir -t deb -n gestic-usb-calibrator -v 0.0.1 -a armhf -C rootfs etc

ninja-fix-applicator_0.1.0~trustyspheramid-8_armhf.deb:
	wget http://s3.amazonaws.com/ninjablocks-apt-repo/pool/main/n/ninja-fix-applicator/ninja-fix-applicator_0.1.0~trustyspheramid-8_armhf.deb
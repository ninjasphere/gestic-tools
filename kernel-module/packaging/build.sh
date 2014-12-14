#!/bin/bash

cd $(dirname $0)/../

make clean
make

rm -rf .tmp-pkg
mkdir -p .tmp-pkg/lib/modules/generic/extra
mkdir -p .tmp-pkg/etc/modprobe.d

cp gestic.ko .tmp-pkg/lib/modules/generic/extra/gestic.ko
cp packaging/modprobe-hax.conf .tmp-pkg/etc/modprobe.d/gestic.conf

PACKAGE_NAME=gestic-driver
PACKAGE_VERSION=1.0.0
PACKAGE_ARCH=armhf

fpm -s dir -t deb -a ${PACKAGE_ARCH} -n ${PACKAGE_NAME} -v ${PACKAGE_VERSION} --after-install packaging/kernel-postinstall.sh --iteration raw -C .tmp-pkg/ lib etc

rm -rf .tmp-pkg

if [[ ! -z "$PUSH_SCRIPT" ]]; then
	$PUSH_SCRIPT $(pwd)/${PACKAGE_NAME}_${PACKAGE_VERSION}-raw_${PACKAGE_ARCH}.deb
fi
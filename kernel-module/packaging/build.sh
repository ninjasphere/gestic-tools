#!/bin/bash

cd $(dirname $0)/../

make clean
make

rm -rf .tmp-pkg
mkdir -p .tmp-pkg/lib/modules/3.12.10+/extra

cp gestic.ko .tmp-pkg/lib/modules/3.12.10+/extra/gestic.ko

PACKAGE_NAME=gestic-driver
PACKAGE_VERSION=1.0.0
PACKAGE_ARCH=armhf

fpm -s dir -t deb -a ${PACKAGE_ARCH} -n ${PACKAGE_NAME} -v ${PACKAGE_VERSION} --after-install packaging/kernel-postinstall.sh --iteration raw -C .tmp-pkg/ lib

rm -rf .tmp-pkg

if [[ ! -z "$PUSH_SCRIPT" ]]; then
	$PUSH_SCRIPT $(pwd)/${PACKAGE_NAME}_${PACKAGE_VERSION}_${PACKAGE_ARCH}.deb
fi
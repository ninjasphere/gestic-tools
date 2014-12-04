#!/bin/bash

cd $(dirname $0)/../

make clean
make

rm -rf .tmp-pkg
mkdir -p .tmp-pkg/lib/modules/3.12.10+/extra

cp gestic.ko .tmp-pkg/lib/modules/3.12.10+/extra/gestic.ko

fpm -s dir -t deb -a armhf -n gestic-driver -v 0.0.1 --after-install packaging/kernel-postinstall.sh -C .tmp-pkg/ lib

rm -rf .tmp-pkg
#!/bin/bash

cd $(dirname $0)

make clean
make

# clean cruft
rm -rf .pkg/lib/modules/*/modules.*

fpm -s dir -t deb -n gestic-driver -v 0.0.1 -C .pkg/ lib
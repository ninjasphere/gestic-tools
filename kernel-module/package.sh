#!/bin/bash

cd $(dirname $0)

make clean
make

fpm -s dir -t deb -n gestic-driver -v 0.0.1 -C .pkg/

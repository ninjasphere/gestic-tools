@echo off
echo "Extracting Library.c from %1"
mgcconv.exe -n Library -w library %1 Library.c
echo "Extracting Loader.c from %1"
mgcconv.exe -n Loader -w loader %1 Loader.c

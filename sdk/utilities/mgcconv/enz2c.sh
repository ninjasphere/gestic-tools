rm Loader.c Library.c
wine mgcconv.exe -w loader $1 Loader.c
wine mgcconv.exe -w library $1 Library.c

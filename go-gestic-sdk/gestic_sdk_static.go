// +build linux

package gestic

/*
#cgo CFLAGS: -I../sdk/api/include -DGESTIC_HAS_DYNAMIC
#include <gestic_api.h>
#include "../sdk/api/src/core.c"
#include "../sdk/api/src/flash.c"
#include "../sdk/api/src/fw_version.c"
#include "../sdk/api/src/rtc.c"
#include "../sdk/api/src/stream.c"
#include "../sdk/api/src/io/cdserial_linux.c"
#include "../sdk/api/src/io/serial.c"
#include "../sdk/api/src/dynamic/depr_stream.c"
#include "../sdk/api/src/dynamic/dynamic.c"
*/
import "C"

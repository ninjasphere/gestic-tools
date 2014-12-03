package gestic

/*
#cgo CFLAGS: -I../sdk/api/include -DGESTIC_HAS_DYNAMIC
#include <gestic_api.h>
*/
import "C"

import (
	"errors"
	"unsafe"
	"strings"
)

type GestIC struct {
	impl *C.gestic_t
}

func Version() string {
	return C.GoString(C.gestic_version_str())
}

func Open() (GestureInterface, error) {
	g := new(GestIC)

	g.impl = C.gestic_create()
	if g.impl == nil {
		return nil, errors.New("Could not allocate gestic_t instance")
	}

	C.gestic_initialize(g.impl)

	res := C.gestic_open(g.impl)
	if res != C.GESTIC_NO_ERROR {
		return nil, errors.New("Could not connect to GestIC device")
	}

	return g, nil
}

func (g *GestIC) Close() {
	C.gestic_close(g.impl)
	C.gestic_cleanup(g.impl)
	C.gestic_free(g.impl)
	g.impl = nil
}

func (g *GestIC) FirmwareVersion() (string, error) {
	buf := make([]byte, 120)
	
	cstr := (*C.char)(unsafe.Pointer(&buf[0]))
	errno := C.gestic_query_fw_version(g.impl, cstr, C.int(len(buf)), 100)
	if errno != C.GESTIC_NO_ERROR {
		return "", errors.New("Could not read firmware version")
	}

	ver := string(buf)
	trimmed := ver[:strings.Index(ver, "\x00")]
	
	return trimmed, nil
}

// +build linux

package gestic

/*
#cgo CFLAGS: -I../sdk/api/include -DGESTIC_HAS_DYNAMIC
#include <gestic_api.h>
*/
import "C"

import (
	"errors"
	"fmt"
	"log"
	"strings"
	"time"
	"unsafe"
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

	// default to everything. TODO: add func to configure this
	C.gestic_set_output_enable_mask(g.impl, C.gestic_data_mask_all, C.gestic_data_mask_all, C.gestic_data_mask_all, 100)

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

func (g *GestIC) getCurrentMessage() GestureMessage {
	msg := GestureMessage{
		Time: time.Now(),
	}

	cic := C.gestic_get_cic(g.impl, 0)
	dev := C.gestic_get_sd(g.impl, 0)
	for i := 0; i < 5; i++ {
		msg.RawCICSignals.Channels[i] = float32(cic.channel[i])
		msg.SignalDeviation.Channels[i] = float32(dev.channel[i])
	}

	pos := C.gestic_get_position(g.impl, 0)
	msg.Position.X = int(pos.x)
	msg.Position.Y = int(pos.y)
	msg.Position.Z = int(pos.z)

	ges := C.gestic_get_gesture(g.impl, 0)
	msg.Gesture.Gesture = GestureType(ges.gesture)
	msg.Gesture.EdgeFlick = ((ges.flags & C.gestic_gesture_edge_flick) != 0)
	msg.Gesture.InProgress = ((ges.flags & C.gestic_gesture_in_progress) != 0)
	msg.Gesture.CountSinceLast = int(ges.last_event)

	tch := C.gestic_get_touch(g.impl, 0)
	msg.Touch.North = ((tch.flags & C.gestic_touch_north) != 0)
	msg.Touch.South = ((tch.flags & C.gestic_touch_south) != 0)
	msg.Touch.East = ((tch.flags & C.gestic_touch_east) != 0)
	msg.Touch.West = ((tch.flags & C.gestic_touch_west) != 0)
	msg.Touch.Center = ((tch.flags & C.gestic_touch_center) != 0)
	msg.Touch.CountSinceLast = int(tch.last_event)

	msg.Tap.North = ((tch.tap_flags & C.gestic_tap_north) != 0)
	msg.Tap.South = ((tch.tap_flags & C.gestic_tap_south) != 0)
	msg.Tap.East = ((tch.tap_flags & C.gestic_tap_east) != 0)
	msg.Tap.West = ((tch.tap_flags & C.gestic_tap_west) != 0)
	msg.Tap.Center = ((tch.tap_flags & C.gestic_tap_center) != 0)
	msg.Tap.CountSinceLast = int(tch.last_tap_event)

	msg.DoubleTap.North = ((tch.tap_flags & C.gestic_double_tap_north) != 0)
	msg.DoubleTap.South = ((tch.tap_flags & C.gestic_double_tap_south) != 0)
	msg.DoubleTap.East = ((tch.tap_flags & C.gestic_double_tap_east) != 0)
	msg.DoubleTap.West = ((tch.tap_flags & C.gestic_double_tap_west) != 0)
	msg.DoubleTap.Center = ((tch.tap_flags & C.gestic_double_tap_center) != 0)
	msg.DoubleTap.CountSinceLast = int(tch.last_tap_event)

	air := C.gestic_get_air_wheel(g.impl, 0)
	msg.AirWheel.Counter = int(air.counter)
	msg.AirWheel.Active = (air.active != 0)
	msg.AirWheel.CountSinceLast = int(air.last_event)

	return msg
}

func (g *GestIC) dataStreamUpdate() (error, bool) {
	res := C.gestic_data_stream_update(g.impl, nil)

	switch res {
	case C.GESTIC_NO_ERROR:
		return nil, false
	case C.GESTIC_NO_DATA:
		return fmt.Errorf("No data."), false
	default:
		return fmt.Errorf("Error while updating data stream: %d", res), true
	}
}

func (g *GestIC) DataStream() chan GestureMessage {
	c := make(chan GestureMessage, 16)

	go func() {
		for {
			err, fatal := g.dataStreamUpdate()
			if err != nil {
				if fatal {
					panic(err)
				} else {
					log.Printf("warning: %v", err)
					continue
				}
			}
			c <- g.getCurrentMessage()
		}
	}()

	return c
}

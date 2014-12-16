package gestic

import (
	"fmt"
	"strings"
	"time"
)

type GestureInterface interface {
	Close()
	FirmwareVersion() (string, error)
	DataStream() chan GestureMessage
}

type GestureMessage struct {
	Time            time.Time
	RawCICSignals   Signal
	SignalDeviation Signal
	Position        Position
	Gesture         Gesture
	Touch           Location
	Tap             Location
	DoubleTap       Location
	AirWheel        AirWheel
	//Calibration
	//Frequency
	//Noise Power
}

type Signal struct {
	Channels [5]float32
}

type Position struct {
	X int
	Y int
	Z int
}

type Gesture struct {
	Gesture        GestureType
	EdgeFlick      bool
	InProgress     bool
	CountSinceLast int
}

type GestureType int

const (
	GestureNone                   = 0x00
	GestureFlickWestToEast        = 0x01
	GestureFlickEastToWest        = 0x02
	GestureFlickSouthToNorth      = 0x03
	GestureFlickNorthToSouth      = 0x04
	GestureCircleClockwise        = 0x05
	GestureCircleCounterClockwise = 0x06
)

type Location struct {
	North          bool
	East           bool
	South          bool
	West           bool
	Center         bool
	CountSinceLast int
}

type AirWheel struct {
	Counter        int
	Active         bool
	CountSinceLast int
}

func (s Signal) String() string {
	return fmt.Sprintf("[%f,%f,%f,%f,%f]", s.Channels[0], s.Channels[1], s.Channels[2], s.Channels[3], s.Channels[4])
}

func (p Position) String() string {
	return fmt.Sprintf("{X:%d Y:%d Z:%d}", p.X, p.Y, p.Z)
}

func (g GestureType) String() string {
	switch g {
	case GestureNone:
		return "GestureNone"
	case GestureFlickWestToEast:
		return "GestureFlickWestToEast"
	case GestureFlickEastToWest:
		return "GestureFlickEastToWest"
	case GestureFlickSouthToNorth:
		return "GestureFlickSouthToNorth"
	case GestureFlickNorthToSouth:
		return "GestureFlickNorthToSouth"
	case GestureCircleClockwise:
		return "GestureCircleClockwise"
	case GestureCircleCounterClockwise:
		return "GestureCircleCounterClockwise"
	}
	return "Unknown"
}

func (g Gesture) String() string {
	return fmt.Sprintf("{Gesture:%s EdgeFlick:%s InProgress:%s}", g.Gesture, g.EdgeFlick, g.InProgress)
}

func (l Location) String() string {
	s := make([]string, 5)
	i := 0
	if l.North {
		s[i] = "North"
		i += 1
	}
	if l.South {
		s[i] = "South"
		i += 1
	}
	if l.East {
		s[i] = "East"
		i += 1
	}
	if l.West {
		s[i] = "West"
		i += 1
	}
	if l.Center {
		s[i] = "Center"
		i += 1
	}
	return fmt.Sprintf("{%s}", strings.Join(s[:i], ", "))
}

func (l Location) Active() bool {
	return l.North || l.South || l.East || l.West || l.Center
}

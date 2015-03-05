// +build darwin

package gestic

import "time"

type GestIC struct {
}

func Version() string {
	return "FAKE"
}

func Open() (GestureInterface, error) {
	return &GestIC{}, nil
}

func (g *GestIC) Close() {
}

func (g *GestIC) FirmwareVersion() (string, error) {
	return "FAKE", nil
}

func (g *GestIC) getCurrentMessage() GestureMessage {
	msg := GestureMessage{}

	msg.Position.X = 100
	msg.Position.Y = 100
	msg.Position.Z = 100

	return msg
}

func (g *GestIC) dataStreamUpdate() error {
	return nil
}

func (g *GestIC) DataStream() chan GestureMessage {
	c := make(chan GestureMessage, 16)

	go func() {
		for {
			time.Sleep(time.Millisecond * 100)
			c <- g.getCurrentMessage()
		}
	}()

	return c
}

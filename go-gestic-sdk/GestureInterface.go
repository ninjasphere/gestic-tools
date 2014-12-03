package gestic

type GestureInterface interface {
	Close()
	FirmwareVersion() (string, error)
}

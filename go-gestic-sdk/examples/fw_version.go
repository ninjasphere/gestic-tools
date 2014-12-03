package main

import (
	"github.com/ninjasphere/gestic-tools/go-gestic-sdk"
	"log"
)

func main() {
	g, err := gestic.Open()
	if err != nil {
		log.Fatalf("Could not connect to GestIC device: %s\n", err)
	}
	defer g.Close()

	log.Println("Succesfully connected to GestIC device")

	ver, err := g.FirmwareVersion()
	if err != nil {
		log.Fatalf("Could not get version: %s\n", err)
	}

	log.Printf("Device firmware version: %s\n", ver)
}



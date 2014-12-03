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
	
	stream := g.DataStream()

	for {
		event := <- stream
		
		log.Printf("Pos: %s\t\tTouch:%s", event.Position, event.Touch)
	}
}



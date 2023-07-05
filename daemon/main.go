package main

import (
	"fmt"
	gc "github.com/CalebDepatie/go-common"
	"net"
	"os"
)

func main() {
	socketFile := "/var/run/mash.sock"

	// setup socket for listening
	listener, err := net.Listen("unix", socketFile)
	if err != nil {
		gc.LogFatal("Error Creating Socket:", err)
	}

	defer listener.Close()

	err = os.Chmod(socketFile, 0777)
	if err != nil {
		gc.LogFatal("Error Setting Socket Perms:", err)
	}

	// Run execution for connections
	for {
		conn, err := listener.Accept()
		if err != nil {
			gc.LogError("Error Accepting Connection:", err)
			continue
		}

		go handleConnection(conn)
	}
}

func handleConnection(conn net.Conn) {
	defer conn.Close()

	// Parse metadata

	fmt.Println("Hello World")
}

package main

import (
	"io"
	"net"
	"os"
	sig "os/signal"
	"syscall"

	gc "github.com/CalebDepatie/go-common"
	es "github.com/CalebDepatie/mash/execStream"
	"google.golang.org/protobuf/proto"
)

func main() {
	socketFile := "/var/run/mash.sock"

	// setup socket for listening
	listener, err := net.Listen("unix", socketFile)
	if err != nil {
		gc.LogError("Error Creating Socket:", err)
		return
	}

	signalChan := make(chan os.Signal, 1)
	sig.Notify(signalChan, os.Interrupt, syscall.SIGTERM)

	// socket cleanup
	go func() {
		<-signalChan
		cleanupSocketFile(socketFile)
		os.Exit(0)
	}()

	defer cleanupSocketFile(socketFile)
	defer listener.Close()

	err = os.Chmod(socketFile, 0777)
	if err != nil {
		gc.LogError("Error Setting Socket Perms:", err)
		return
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

func cleanupSocketFile(path string) {
	err := os.Remove(path)
	if err != nil {
		gc.LogFatal("Error Deleting Socket File:", err)
	}
}

func handleConnection(conn net.Conn) {
	defer conn.Close()

	message_home := func(res, err_res string) {
		response := &es.ExecResponse{
			Result: res,
			Error:  err_res,
		}

		out, err := proto.Marshal(response)
		if err != nil {
			gc.LogError("Failed to encode response:", err)
			return
		}

		_, err = conn.Write(out)
		if err != nil {
			gc.LogError("Failed to send response:", err)
			return
		}
	}

	// gracefully handle a runtime error
	defer func() {
		if r := recover(); r != nil {
			gc.LogError("Daemon Error:", r)
			message_home("", r.(error).Error())
		}
	}()

	// parse data
	buffer := make([]byte, 1024) // choose an appropriate buffer size
	message := []byte{}
	for {
		bytesRead, err := conn.Read(buffer)
		if err != nil {
			if err != io.EOF {
				gc.LogError("Failed to read incoming connection data:", err)
			}
			break
		}

		message = append(message, buffer[:bytesRead]...)
	}

	program := &es.ExecStream{}
	err := proto.Unmarshal(message, program)
	if err != nil {
		gc.LogError("Failed to parse execution stream:", err)
		return
	}

	// exec := run.NewExecutor(program)

	// send response back home
	message_home("", "NYI")
}

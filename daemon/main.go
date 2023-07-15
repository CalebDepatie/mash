package main

import (
	gc "github.com/CalebDepatie/go-common"
	es "github.com/CalebDepatie/mash/execStream"
	"google.golang.org/protobuf/proto"
	"io"
	"net"
	"os"
	sig "os/signal"
	"syscall"
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

// todo: some abstractions here would probably be nice
func handleConnection(conn net.Conn) {
	defer conn.Close()

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

	// execute
	executor := NewExecutor(program.CurrentWorkingDir)
	result := ""

	for _, key := range program.GetExecKeys() {
		switch key.Op {
		case es.Operation_StringVal:
			{
				executor.PushVal(StringValue{key.GetStringValue()})
			}
		case es.Operation_NumberVal:
			{
				executor.PushVal(DoubleValue{key.GetNumberValue()})
			}
		case es.Operation_BoolVal:
			{
				executor.PushVal(BoolValue{key.GetBooleanValue()})
			}
		case es.Operation_RangeVal:
			{
				r := key.GetRangeValue()
				executor.PushVal(RangeValue{[2]int32{r.From, r.To}})
			}
		case es.Operation_Recall:
			{
				executor.PushVal(IdenValue{key.GetStringValue()})
			}

		case es.Operation_ClearReg:
			{
				res_val := executor.Exec()
				result += res_val.String() + "\n"
			}
		case es.Operation_ScopeStart:
			{
				executor.stack.NewLayer()
			}
		case es.Operation_ScopeEnd:
			{
				_ = executor.stack.PopLayer()
			}
		case es.Operation_Begin:
			{
				// todo
			}

		default:
			{
				executor.PushOp(Operation{
					key.Op,
					key.GetStringValue(),
				})
			}
		}
	}

	// send response back home
	res := &es.ExecResponse{
		Result: result,
	}

	out, err := proto.Marshal(res)
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

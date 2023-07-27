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

	// execute
	executor := NewExecutor(program.CurrentWorkingDir)

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
				executor.AddLineEnd()
			}

		// case es.Operation_ClearReg:
		// 	{
		// 		res_val := executor.Exec()
		// 		result += res_val.String() + "\n"
		// 	}
		// case es.Operation_ScopeStart:
		// 	{
		// 		executor.stack.NewLayer()
		// 		// executor.PushOp(Operation{)
		// 	}
		// case es.Operation_ScopeEnd:
		// 	{
		// 		_ = executor.stack.PopLayer()
		// 	}
		case es.Operation_Begin:
			{
				// Will be used to indicate when a continuous stream of operations
				// is being sent rather than a full file
				// REPL vs Script eval
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
	message_home(executor.Exec(), "")
}

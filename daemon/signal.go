package main

import (
	gc "github.com/CalebDepatie/go-common"
	"sync"
)

type signal struct {
	ch        chan string
	listeners int
	lis_mut   sync.Mutex
}

var signal_list map[string]signal

func init() {
	signal_list = make(map[string]signal)
}

func createSignal(id string) {
	new_signal := signal{
		ch:        make(chan string, 1),
		listeners: 0,
	}

	signal_list[id] = new_signal

	gc.LogInfo("New Signal Created", id)
}

func ListSignals() []string {
	keys := make([]string, 0, len(signal_list))
	for k := range signal_list {
		keys = append(keys, k)
	}

	return keys
}

func Accept(id string) string {
	// create if it doesnt exist
	sig, exists := signal_list[id]

	if !exists {
		createSignal(id)
		sig = signal_list[id]
	}

	sig.lis_mut.Lock()
	sig.listeners++
	sig.lis_mut.Unlock()

	return <-sig.ch
}

func Notify(id, msg string) {
	sig := signal_list[id]

	// attempt to broadcast
	sig.lis_mut.Lock()
	for sig.listeners > 0 {
		sig.ch <- msg

		sig.listeners--
	}
	sig.lis_mut.Unlock()

	// purge signal?
}

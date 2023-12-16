package runtime

import "testing"

// ERROR: I'm occassionally getting concurrent map writess

func TestOneSignal(t *testing.T) {
	go func() {
		ret := Accept("test")

		if ret != "foo" {
			t.Errorf("Signal Accept was not expected: %s", ret)
		}
	}()

	Notify("test", "foo")
}

func TestBroadcastSignal(t *testing.T) {
	listener := func() {
		ret := Accept("test")

		if ret != "foo" {
			t.Errorf("Signal Accept was not expected: %s", ret)
		}
	}

	for i := 0; i < 10; i++ {
		go listener()
	}

	Notify("test", "foo")
}

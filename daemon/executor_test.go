package main

import "testing"

func TestExecutorMath(t *testing.T) {
	executor := NewExecutor()

	// operations come in op left (right, value or nested math)
	// |-> 10 / 5
	executor.PushOp(Div)
	executor.PushVal(10)
	executor.PushVal(5)

	ret := executor.Exec()
	if ret != 2 {
		t.Errorf("Error executing math. Expected %d, Recieved: %f", 2, ret)
	}

	// Time to test a more complex operation, in the way it would be given
	// |-> 5 * 2 + 5 % 3
	executor.PushOp(Mul)
	executor.PushVal(5)
	executor.PushOp(Add)
	executor.PushVal(2)
	executor.PushOp(Mod)
	executor.PushVal(5)
	executor.PushVal(3)

	ret = executor.Exec()
	if ret != 0 {
		t.Errorf("Error executing math. Expected %d, Recieved: %f", 0, ret)
	}
}

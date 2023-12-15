package runtime

import (
	"testing"

	es "github.com/CalebDepatie/mash/execStream"
)

func TestExecutorMath(t *testing.T) {
	executor := NewExecutor("~/")

	// operations come in op left (right, value or nested math)
	// |-> 10 / 5
	executor.PushOp(Operation{es.Operation_Math, "/"})
	executor.PushVal(DoubleValue{10})
	executor.PushVal(DoubleValue{5})

	ret := executor.ExecLine()
	if ret.Double() != 2 {
		t.Errorf("Error executing math. Expected %d, Recieved: %f", 2, ret.Double())
	}

	// Time to test a more complex operation, in the way it would be given
	// |-> 5 * 2 + 5 % 3
	executor.PushOp(Operation{es.Operation_Math, "*"})
	executor.PushVal(DoubleValue{5})
	executor.PushOp(Operation{es.Operation_Math, "+"})
	executor.PushVal(DoubleValue{2})
	executor.PushOp(Operation{es.Operation_Math, "%"})
	executor.PushVal(DoubleValue{5})
	executor.PushVal(DoubleValue{3})

	ret = executor.ExecLine()
	if ret.Double() != 0 {
		t.Errorf("Error executing math. Expected %d, Recieved: %f", 0, ret.Double())
	}
}

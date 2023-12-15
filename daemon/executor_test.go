package main

import (
	"testing"

	es "github.com/CalebDepatie/mash/execStream"
	run "github.com/CalebDepatie/mash/runtime"
)

func TestExecutorMath(t *testing.T) {

	// operations come in op left (right, value or nested math)
	// |-> 10 / 5

	actions := []Action{
		{OperationAction, run.NilValue{}, Operation{es.Operation_Math, "/"}},
		{ValueAction, run.DoubleValue{10}, Operation{}},
		{ValueAction, run.DoubleValue{5}, Operation{}},
	}

	executor := NewExecutor("~/", actions)

	ret := executor.ExecLine()
	if ret.Double() != 2 {
		t.Errorf("Error executing math. Expected %d, Recieved: %f", 2, ret.Double())
	}

	// Time to test a more complex operation, in the way it would be given
	// |-> 5 * 2 + 5 % 3
	actions = []Action{
		{OperationAction, run.NilValue{}, Operation{es.Operation_Math, "*"}},
		{ValueAction, run.DoubleValue{5}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_Math, "+"}},
		{ValueAction, run.DoubleValue{2}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_Math, "%"}},
		{ValueAction, run.DoubleValue{5}, Operation{}},
		{ValueAction, run.DoubleValue{3}, Operation{}},
	}

	executor = NewExecutor("~/", actions)

	ret = executor.ExecLine()
	if ret.Double() != 0 {
		t.Errorf("Error executing math. Expected %d, Recieved: %f", 0, ret.Double())
	}
}

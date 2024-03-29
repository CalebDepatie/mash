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
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},
	}

	executor := NewExecutor("~/", actions)

	ret := executor.StartExecution()
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
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},
	}

	executor = NewExecutor("~/", actions)

	ret = executor.StartExecution()
	if ret.Double() != 0 {
		t.Errorf("Error executing math. Expected %d, Recieved: %f", 0, ret.Double())
	}
}

func TestExecutorAsmt(t *testing.T) {
	actions := []Action{
		{OperationAction, run.NilValue{}, Operation{es.Operation_Asmt, "test"}},
		{ValueAction, run.DoubleValue{5}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},
		{ValueAction, run.IdenValue{"test"}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},
	}

	executor := NewExecutor("~/", actions)

	ret := executor.StartExecution()

	if ret.Double() != 5 {
		t.Errorf("Error executing asmt. Expected %d, Recieved: %f", 10, ret.Double())
	}
}

func TestExecutorAsmtMath(t *testing.T) {
	actions := []Action{
		{OperationAction, run.NilValue{}, Operation{es.Operation_Asmt, "test"}},
		{ValueAction, run.DoubleValue{5}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_Asmt, "test"}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_Math, "+"}},
		{ValueAction, run.IdenValue{"test"}, Operation{}},
		{ValueAction, run.IdenValue{"test"}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},
		{ValueAction, run.IdenValue{"test"}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},
	}

	executor := NewExecutor("~/", actions)

	ret := executor.StartExecution()

	if ret.Double() != 10 {
		t.Errorf("Error executing asmt math. Expected %d, Recieved: %f", 10, ret.Double())
	}
}

func TestExecutorCond(t *testing.T) {
	actions := []Action{
		{ValueAction, run.DoubleValue{5}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_Cond, "<"}},
		{ValueAction, run.DoubleValue{6}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},
	}

	executor := NewExecutor("~/", actions)

	ret := executor.StartExecution()

	if !ret.Bool() {
		t.Errorf("Error executing cond. Receieved False")
	}
}

func TestExecutorIf(t *testing.T) {
	actions := []Action{
		{OperationAction, run.NilValue{}, Operation{es.Operation_Asmt, "test"}},
		{ValueAction, run.DoubleValue{4}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{OperationAction, run.NilValue{}, Operation{es.Operation_If, ""}},
		{ValueAction, run.DoubleValue{5}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_Cond, "<"}},
		{ValueAction, run.DoubleValue{5}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{OperationAction, run.NilValue{}, Operation{es.Operation_ScopeStart, ""}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{OperationAction, run.NilValue{}, Operation{es.Operation_Asmt, "test"}},
		{ValueAction, run.DoubleValue{16}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{OperationAction, run.NilValue{}, Operation{es.Operation_ScopeEnd, ""}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{ValueAction, run.IdenValue{"test"}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},
	}

	executor := NewExecutor("~/", actions)

	ret := executor.StartExecution()

	if ret.Double() != 4 {
		t.Errorf("Error executing if. Expected %d, Recieved: %f", 4, ret.Double())
	}
}

// TODO: this tests bytecode appears to be incorrect. it only loops once
func TestExecutorLoop(t *testing.T) {
	actions := []Action{
		{OperationAction, run.NilValue{}, Operation{es.Operation_ScopeStart, ""}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{OperationAction, run.NilValue{}, Operation{es.Operation_Asmt, "test"}},
		{ValueAction, run.DoubleValue{0}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{OperationAction, run.NilValue{}, Operation{es.Operation_Loop, ""}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_Asmt, "i"}},
		{ValueAction, run.RangeValue{[2]int32{1, 3}}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{OperationAction, run.NilValue{}, Operation{es.Operation_ScopeStart, ""}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{OperationAction, run.NilValue{}, Operation{es.Operation_Asmt, "test"}},
		{ValueAction, run.IdenValue{"test"}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_Math, "+"}},
		{ValueAction, run.IdenValue{"i"}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{OperationAction, run.NilValue{}, Operation{es.Operation_ScopeEnd, ""}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{ValueAction, run.IdenValue{"test"}, Operation{}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},

		{OperationAction, run.NilValue{}, Operation{es.Operation_ScopeEnd, ""}},
		{OperationAction, run.NilValue{}, Operation{es.Operation_ClearReg, ""}},
	}

	executor := NewExecutor("~/", actions)

	ret := executor.StartExecution()

	if ret.Double() != 6 {
		t.Errorf("Error executing loop. Expected %d, Recieved: %f", 6, ret.Double())
	}
}

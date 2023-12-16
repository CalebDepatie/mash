package main

import (
	gc "github.com/CalebDepatie/go-common"
	es "github.com/CalebDepatie/mash/execStream"
	run "github.com/CalebDepatie/mash/runtime"
	"math"
	"os"
	"os/exec"
)

type Operation struct {
	Op  es.Operation
	Val string
}

// handles right to left execution for a task
type Executor struct {
	// cwd string
	stack     run.StackMap[func(...run.Value) run.Value]
	opQueue   run.Queue[Operation]
	valQueue  run.Queue[run.Value]
	blockStack run.Stack[int]
	script    []Action
	scriptPos int
}

func NewExecutor(cwd string, cmd_ops []Action) Executor {
	new_exec := Executor{
		stack:     run.NewStackMap[func(...run.Value) run.Value](),
		opQueue:   run.NewQueue[Operation](),
		valQueue:  run.NewQueue[run.Value](),
		blockStack: run.NewStack[int](),
		script:   cmd_ops,
		scriptPos: 0,
	}

	new_exec.stack.Set("run", func(args ...run.Value) run.Value {
		shell, ok := os.LookupEnv("SHELL")
		if !ok {
			gc.LogError("Could not get environment variable $SHELL for execution")
			return run.NilValue{}
		}

		command := ""
		for _, arg := range args {

			command += " " + arg.String()
		}

		cmd := exec.Command(shell, "-c", command)
		cmd.Dir = cwd

		stdout, err := cmd.CombinedOutput()

		out := string(stdout)

		if err != nil {
			out += "\n" + err.Error()
		}

		return run.StringValue{out}
	})

	new_exec.stack.Set("accept", func(args ...run.Value) run.Value {
		return run.NilValue{}
	})

	new_exec.stack.Set("notify", func(args ...run.Value) run.Value {
		return run.NilValue{}
	})

	new_exec.stack.Set("fork", func(args ...run.Value) run.Value {
		return run.NilValue{}
	})

	new_exec.stack.Set("join", func(args ...run.Value) run.Value {
		return run.NilValue{}
	})

	return new_exec
}

func (e *Executor) PushOp(op Operation) {
	e.opQueue.PushBack(op)
}

func (e *Executor) PushVal(val run.Value) {
	e.valQueue.PushBack(val)
}

func executeMath(op string, left, right float64) run.DoubleValue {
	var ret float64

	switch op {
	case "+":
		{
			ret = left + right
		}
	case "-":
		{
			ret = left - right
		}
	case "*":
		{
			ret = left * right
		}
	case "/":
		{
			ret = left / right
		}
	case "^":
		{
			ret = math.Pow(left, right)
		}
	case "%":
		{
			ret = math.Mod(left, right)
		}
	}

	return run.DoubleValue{ret}
}

func executeCond(op string, left, right run.Value) run.Value {
	var eval bool

	if left.Type() != right.Type() || left.Type() == run.Nil {
		return run.NilValue{} // for now, lets only check conditionals on the same primitives
	}

	switch op {
	case "<":
		{
			switch left := left.(type) {
			case run.DoubleValue:
				eval = left.Double() < right.Double()
			case run.StringValue:
				eval = left.String() < right.String()
			case run.RangeValue:
				eval = left.Range()[1]-left.Range()[0] < right.Range()[1]-right.Range()[0]
			}
		}
	case "<=":
		{
			switch left := left.(type) {
			case run.DoubleValue:
				eval = left.Double() <= right.Double()
			case run.StringValue:
				eval = left.String() <= right.String()
			case run.RangeValue:
				eval = left.Range()[1]-left.Range()[0] <= right.Range()[1]-right.Range()[0]
			}

		}
	case "==":
		{
			switch left := left.(type) {
			case run.BoolValue:
				eval = left.Bool() == right.Bool()
			case run.DoubleValue:
				eval = left.Double() == right.Double()
			case run.StringValue:
				eval = left.String() == right.String()
			case run.RangeValue:
				eval = left.Range()[1] == right.Range()[1] && left.Range()[0] == right.Range()[0]
			}
		}
	case "!=":
		{
			switch left := left.(type) {
			case run.BoolValue:
				eval = left.Bool() != right.Bool()
			case run.DoubleValue:
				eval = left.Double() != right.Double()
			case run.StringValue:
				eval = left.String() != right.String()
			case run.RangeValue:
				eval = left.Range()[1] != right.Range()[1] || left.Range()[0] != right.Range()[0]
			}
		}
	case ">=":
		{
			switch left := left.(type) {
			case run.DoubleValue:
				eval = left.Double() >= right.Double()
			case run.StringValue:
				eval = left.String() >= right.String()
			case run.RangeValue:
				eval = left.Range()[1]-left.Range()[0] >= right.Range()[1]-right.Range()[0]
			}

		}
	case ">":
		{
			switch left := left.(type) {
			case run.DoubleValue:
				eval = left.Double() > right.Double()
			case run.StringValue:
				eval = left.String() > right.String()
			case run.RangeValue:
				eval = left.Range()[1]-left.Range()[0] > right.Range()[1]-right.Range()[0]
			}
		}
	}

	return run.BoolValue{eval}
}

func (e *Executor) recallIfPossible(val run.Value) run.Value {
	if val.Type() != run.Iden {
		return val
	}

	key := val.String()
	determinedValue, err := e.stack.Get(key)

	if err != nil {
		gc.LogWarning("Attemped to access var that doesn't exist:", key)
		return val
	}

	return determinedValue()
}

func valueWrap(val run.Value) func(...run.Value) run.Value {
	return func(args ...run.Value) run.Value {
		return val
	}
}

func (e *Executor) StartExecution() run.Value {
	if len(e.script) == 0 {
		gc.LogError("Called StartExecution with an empty script")
		return run.NilValue{}
	}

	var ret run.Value = run.NilValue{}

	for e.scriptPos < len(e.script) {
		action := e.script[e.scriptPos]

		switch action.GetType() {
		case OperationAction:
			{
				operation := action.GetOperation()

				switch operation.Op {
				case es.Operation_ClearReg:
					{
						ret = e.execOp()
					}
				case es.Operation_ScopeEnd:
					{
						_ = e.stack.PopLayer()
						e.blockStack.Pop()
					}
				case es.Operation_ScopeStart:
					{
						e.stack.NewLayer()
						e.blockStack.Push(e.scriptPos)
					}
					fallthrough
				default:
					{
						e.PushOp(operation)
					}
				}				
			}
		case ValueAction:
			{
				e.PushVal(action.GetValue())
			}
		}

		e.scriptPos++
	}

	return ret
}

func (e *Executor) execOp() run.Value {

	var ret run.Value = run.NilValue{}

	for !e.opQueue.IsEmpty() {
		op, _ := e.opQueue.PopFront()

		switch op.Op {
		case es.Operation_Math:
			{
				var (
					left, right run.Value
				)

				if ret.Type() == run.Nil {
					left, _ = e.valQueue.PopFront()
					right, _ = e.valQueue.PopFront()

				} else {
					left = ret
					right, _ = e.valQueue.PopFront()
				}

				left = e.recallIfPossible(left)
				right = e.recallIfPossible(right)

				ret = executeMath(op.Val, left.Double(), right.Double())

				gc.LogInfo("Math Op", left, op.Val, right, ret)
			}

		case es.Operation_Asmt:
			{
				if ret.Type() == run.Nil {
					ret, _ = e.valQueue.PopFront()
				}

				e.stack.Set(op.Val, valueWrap(ret))
			}

		case es.Operation_Recall:
			{
				// NOTE: Assuming this is just a wrapped value
				value_func, _ := e.stack.Get(op.Val)
				ret = value_func()
			}

		default:
			{
				gc.LogWarning("Token unevaluated", op.Op)
				ret = run.NilValue{}
			}

		}
	}

	return ret
}
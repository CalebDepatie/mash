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
	lines     run.Queue[[2]int]
	blockStack run.Stack[int]
	script    []Action
	scriptPos int
}

func NewExecutor(cwd string, cmd_ops []Action) Executor {
	new_exec := Executor{
		stack:     run.NewStackMap[func(...run.Value) run.Value](),
		opQueue:   run.NewQueue[Operation](),
		valQueue:  run.NewQueue[run.Value](),
		lines:     run.NewQueue[[2]int](),
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

func (e *Executor) AddLineEnd() {

	e.lines.PushBack([2]int{e.opQueue.Length(), e.valQueue.Length()})
}

// determines if its at the end of a line in the (opQueue and valQueue)
func (e *Executor) IsEnd() (bool, bool) {
	if e.lines.IsEmpty() {
		return true, true
	}

	bottom := e.lines.PeekBottom()
	opEnd, valEnd := bottom[0], bottom[1]

	return e.opQueue.Index() == opEnd, e.valQueue.Index() == valEnd
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

// handles full execution. Will return the whole script result at once
func (e *Executor) Exec() string {
	result := ""

	for !e.opQueue.IsEmpty() && !e.lines.IsEmpty() {

		// Check for ops that won't clear a line
		bottom := e.opQueue.PeekBottom()

		if bottom.Op == es.Operation_ScopeStart {
			e.stack.NewLayer()
			e.opQueue.PopFront()

		} else if bottom.Op == es.Operation_ScopeEnd {
			_ = e.stack.PopLayer()
			e.opQueue.PopFront()

		} else {
			result += e.ExecLine().String() + "\n"

			if !e.lines.IsEmpty() {
				e.lines.PopFront()
			}
		}
	}

	return result
}

// Will execute a full line
func (e *Executor) ExecLine() run.Value {
	if e.opQueue.IsEmpty() {
		gc.LogError("Called Exec with an empty op queue")
		return run.NilValue{}
	}

	// Checks for special case beginnings before executing the line
	// todo: remove this whole special case mess

	bottom := e.opQueue.PeekBottom()
	gc.LogInfo("bottom", bottom.Op)
	
	switch bottom.Op {
	case es.Operation_FnCall:
		{
			args := []run.Value{}

			_, valEnd := e.IsEnd()
			for !e.valQueue.IsEmpty() && !valEnd {
				val, _ := e.valQueue.PopFront()
				args = append(args, e.recallIfPossible(val))

				_, valEnd = e.IsEnd()
			}

			fn, err := e.stack.Get(bottom.Val)

			if err != nil {
				return run.NilValue{}
			}

			return fn(args...)
		}

	case es.Operation_If:
		{
			e.opQueue.PopFront() // pop If key

			op, _ := e.opQueue.PopFront()
			left, _ := e.valQueue.PopFront()
			right, _ := e.valQueue.PopFront()

			if op.Op != es.Operation_Cond {
				return run.NilValue{}
			}

			left = e.recallIfPossible(left)
			right = e.recallIfPossible(right)

			cond := executeCond(op.Val, left, right)
			_ = cond

			// handle scope

		}
	}

	ret := execValueOp(e)

	return ret
}

func execValueOp(e *Executor) run.Value {
	var ret run.Value
	ret_initialized := false

	opEnd, valEnd := e.IsEnd()
	for !e.opQueue.IsEmpty() && !(opEnd && valEnd) {
		op, _ := e.opQueue.PopFront()

		switch op.Op {
		case es.Operation_Math:
			{
				var (
					left, right run.Value
				)

				if ret_initialized {
					left = ret
					right, _ = e.valQueue.PopFront()

				} else {
					left, _ = e.valQueue.PopFront()
					right, _ = e.valQueue.PopFront()
					ret_initialized = true
				}

				left = e.recallIfPossible(left)
				right = e.recallIfPossible(right)

				ret = executeMath(op.Val, left.Double(), right.Double())
			}

		case es.Operation_Asmt:
			{
				if !ret_initialized {
					ret, _ = e.valQueue.PopFront()

					ret_initialized = true
				}

				e.stack.Set(op.Val, valueWrap(ret))
			}

		default:
			{
				gc.LogInfo("skipped", op.Op)
				ret = run.NilValue{}
			}

		}

		opEnd, valEnd = e.IsEnd()
	}

	return ret
}

package main

import (
	gc "github.com/CalebDepatie/go-common"
	es "github.com/CalebDepatie/mash/execStream"
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
	stack     StackMap[func(...Value) Value]
	opQueue   Queue[Operation]
	valQueue  Queue[Value]
	lines     Queue[[2]int]
	skipScope bool
	// loopStack Stack[int]
}

func NewExecutor(cwd string) Executor {
	new_exec := Executor{
		stack:     NewStackMap[func(...Value) Value](),
		opQueue:   NewQueue[Operation](),
		valQueue:  NewQueue[Value](),
		skipScope: false,
		// loopStack: NewStack[int](),
	}

	new_exec.stack.Set("run", func(args ...Value) Value {
		shell, ok := os.LookupEnv("SHELL")
		if !ok {
			gc.LogError("Could not get environment variable $SHELL for execution")
			return NilValue{}
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

		return StringValue{out}
	})

	new_exec.stack.Set("accept", func(args ...Value) Value {
		return NilValue{}
	})

	new_exec.stack.Set("notify", func(args ...Value) Value {
		return NilValue{}
	})

	new_exec.stack.Set("fork", func(args ...Value) Value {
		return NilValue{}
	})

	new_exec.stack.Set("join", func(args ...Value) Value {
		return NilValue{}
	})

	return new_exec
}

func (e *Executor) AddLineEnd() {

	e.lines.PushBack([2]int{e.opQueue.Length(), e.valQueue.Length()})
}

// determines if its at the end of a line in the (opQueue and valQueue)
func (e *Executor) IsEnd() (bool, bool) {
	if e.lines.isEmpty() {
		return true, true
	}

	bottom := e.lines.PeekBottom()
	opEnd, valEnd := bottom[0], bottom[1]

	return e.opQueue.Index() == opEnd, e.valQueue.Index() == valEnd
}

func (e *Executor) PushOp(op Operation) {
	e.opQueue.PushBack(op)
}

func (e *Executor) PushVal(val Value) {
	e.valQueue.PushBack(val)
}

func executeMath(op string, left, right float64) DoubleValue {
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

	return DoubleValue{ret}
}

func executeCond(op string, left, right Value) Value {
	var eval bool

	if left.Type() != right.Type() || left.Type() == Nil {
		return NilValue{} // for now, lets only check conditionals on the same primitives
	}

	switch op {
	case "<":
		{
			switch left := left.(type) {
			case DoubleValue:
				eval = left.Double() < right.Double()
			case StringValue:
				eval = left.String() < right.String()
			case RangeValue:
				eval = left.Range()[1]-left.Range()[0] < right.Range()[1]-right.Range()[0]
			}
		}
	case "<=":
		{
			switch left := left.(type) {
			case DoubleValue:
				eval = left.Double() <= right.Double()
			case StringValue:
				eval = left.String() <= right.String()
			case RangeValue:
				eval = left.Range()[1]-left.Range()[0] <= right.Range()[1]-right.Range()[0]
			}

		}
	case "==":
		{
			switch left := left.(type) {
			case BoolValue:
				eval = left.Bool() == right.Bool()
			case DoubleValue:
				eval = left.Double() == right.Double()
			case StringValue:
				eval = left.String() == right.String()
			case RangeValue:
				eval = left.Range()[1] == right.Range()[1] && left.Range()[0] == right.Range()[0]
			}
		}
	case "!=":
		{
			switch left := left.(type) {
			case BoolValue:
				eval = left.Bool() != right.Bool()
			case DoubleValue:
				eval = left.Double() != right.Double()
			case StringValue:
				eval = left.String() != right.String()
			case RangeValue:
				eval = left.Range()[1] != right.Range()[1] || left.Range()[0] != right.Range()[0]
			}
		}
	case ">=":
		{
			switch left := left.(type) {
			case DoubleValue:
				eval = left.Double() >= right.Double()
			case StringValue:
				eval = left.String() >= right.String()
			case RangeValue:
				eval = left.Range()[1]-left.Range()[0] >= right.Range()[1]-right.Range()[0]
			}

		}
	case ">":
		{
			switch left := left.(type) {
			case DoubleValue:
				eval = left.Double() > right.Double()
			case StringValue:
				eval = left.String() > right.String()
			case RangeValue:
				eval = left.Range()[1]-left.Range()[0] > right.Range()[1]-right.Range()[0]
			}
		}
	}

	return BoolValue{eval}
}

func (e *Executor) recallIfPossible(val Value) Value {
	if val.Type() != Iden {
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

func valueWrap(val Value) func(...Value) Value {
	return func(args ...Value) Value {
		return val
	}
}

// handles full execution. Will return the whole script result at once
func (e *Executor) Exec() string {
	result := ""

	for !e.opQueue.isEmpty() && !e.lines.isEmpty() {

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

			if !e.lines.isEmpty() {
				e.lines.PopFront()
			}
		}
	}

	return result
}

// Will execute a full line
func (e *Executor) ExecLine() Value {
	if e.opQueue.isEmpty() {
		gc.LogError("Called Exec with an empty op queue")
		return NilValue{}
	}

	// Checks for special case beginnings before executing the line
	// todo: remove this whole special case mess

	bottom := e.opQueue.PeekBottom()
	gc.LogInfo("bottom", bottom.Op)
	switch bottom.Op {
	case es.Operation_FnCall:
		{
			args := []Value{}

			_, valEnd := e.IsEnd()
			for !e.valQueue.isEmpty() && !valEnd {
				val, _ := e.valQueue.PopFront()
				args = append(args, e.recallIfPossible(val))

				_, valEnd = e.IsEnd()
			}

			fn, err := e.stack.Get(bottom.Val)

			if err != nil {
				return NilValue{}
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
				return NilValue{}
			}

			left = e.recallIfPossible(left)
			right = e.recallIfPossible(right)

			cond := executeCond(op.Val, left, right)

			// handle scope
			if !cond.Bool() {
				return NilValue{}
			}
		}
	}

	ret := execValueOp(e)

	return ret
}

func execValueOp(e *Executor) Value {
	var ret Value
	ret_initialized := false

	opEnd, valEnd := e.IsEnd()
	for !e.opQueue.isEmpty() && !(opEnd && valEnd) {
		op, _ := e.opQueue.PopFront()

		switch op.Op {
		case es.Operation_Math:
			{
				var (
					left, right Value
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
				ret = NilValue{}
			}

		}

		opEnd, valEnd = e.IsEnd()
	}

	return ret
}

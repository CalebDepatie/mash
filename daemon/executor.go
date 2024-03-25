package main

import (
	"math"
	"os"
	"os/exec"

	gc "github.com/CalebDepatie/go-common"
	es "github.com/CalebDepatie/mash/execStream"
	run "github.com/CalebDepatie/mash/runtime"
)

type Operation struct {
	Op  es.Operation
	Val string
}

// TODO: I would lke to formulate this better
type block struct {
	start int
	loop  bool
}

// handles right to left execution for a task
type Executor struct {
	// cwd string
	stack      run.StackMap[func(...run.Value) run.Value]
	opQueue    run.Queue[Operation]
	valQueue   run.Queue[run.Value]
	blockStack run.Stack[block]
	skipExec   bool
	loopExec   bool
	script     []Action
	scriptPos  int
}

func NewExecutor(cwd string, cmd_ops []Action) Executor {
	new_exec := Executor{
		stack:      run.NewStackMap[func(...run.Value) run.Value](),
		opQueue:    run.NewQueue[Operation](),
		valQueue:   run.NewQueue[run.Value](),
		blockStack: run.NewStack[block](),
		skipExec:   false,
		loopExec:   false,
		script:     cmd_ops,
		scriptPos:  0,
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
	if val == nil {
		gc.LogError("Recalling nil interface value!!")

		return run.NilValue{}
	}

	if val.Type() != run.Iden {
		gc.LogInfo("Recalling Value:", val.String())

		return val
	}

	gc.LogInfo("Recalling Iden:", val.String())

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
		gc.LogWarning("Called StartExecution with an empty script")
		return run.NilValue{}
	}

	var ret run.Value = run.NilValue{}

	for e.scriptPos < len(e.script) {
		action := e.script[e.scriptPos]

		if e.skipExec && action.GetType() == OperationAction {
			operation := action.GetOperation()
			if operation.Op == es.Operation_ScopeEnd {
				e.skipExec = false
			}
		}

		if e.skipExec {
			e.scriptPos++
			continue // can this be a break?
		}

		switch action.GetType() {
		case OperationAction:
			{
				operation := action.GetOperation()

				switch operation.Op {
				case es.Operation_ClearReg:
					{
						line_ret := e.execOp()

						gc.LogInfo("SPECIAL ClearReg", line_ret.String())

						if line_ret.Type() != run.Nil {
							ret = line_ret
						}
					}
				case es.Operation_ScopeEnd:
					{
						gc.LogInfo("SPECIAL ScopeEnd")

						_ = e.stack.PopLayer()
						prev_block := e.blockStack.Pop()

						if prev_block.loop {
							e.scriptPos = prev_block.start - 1

							gc.LogInfo("SPECIAL Looped to", e.scriptPos+1)
						}
					}
				case es.Operation_ScopeStart:
					{
						gc.LogInfo("SPECIAL ScopeStart")

						e.stack.NewLayer()
						e.blockStack.Push(block{e.scriptPos, false})

					}
					// fallthrough
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
				gc.LogInfo("Asmt Op", op.Val, ret)

				// this is called BEFORE execution that would set to ret
				if ret.Type() == run.Nil {
					ret = e.execOp() // parse the next op
				}
				if ret.Type() == run.Nil {
					ret, _ = e.valQueue.PopFront()
				}

				e.stack.Set(op.Val, valueWrap(ret))
			}

		case es.Operation_If:
			{
				// I now know there will be a COND then a BLOCK that may or may not be skipped

				ret = e.execOp()

				e.skipExec = !ret.Bool()

				gc.LogInfo("If Op", op.Val, ret)
			}

		case es.Operation_Cond:
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

				ret = executeCond(op.Val, left, right)

				gc.LogInfo("Cond Op", left, op.Val, right, ret)
			}

		case es.Operation_Loop:
			{
				// asmt of range -> scope
				op_asmt, _ := e.opQueue.PopFront()
				if op_asmt.Op != es.Operation_Asmt {
					gc.LogError("Loop operation must be followed by an assignment")
					return run.NilValue{}
				}

				for_range, _ := e.valQueue.PopFront()
				if for_range.Type() != run.Range {
					gc.LogError("Loop assignment must be a range")
					return run.NilValue{}
				}

				loop_index := e.recallIfPossible(run.IdenValue{op_asmt.Val})

				if !e.loopExec {
					// Initialize to range start on the first iteration

					loop_index = run.DoubleValue{float64(for_range.Range()[0])}
					gc.LogInfo("Loop Op Init", op_asmt.Val, loop_index)

					e.stack.Set(op_asmt.Val, valueWrap(loop_index))

				} else {
					loop_index = run.DoubleValue{loop_index.Double() + 1}
					e.stack.Set(op_asmt.Val, valueWrap(loop_index))
				}

				gc.LogInfo("Loop Op", op.Val, loop_index, for_range)

				if int32(loop_index.Double()) > for_range.Range()[1] {
					e.skipExec = true
					e.loopExec = false

				} else {
					e.loopExec = true
					e.skipExec = false

					// get the loop op start
					for i := e.scriptPos; i >= 0; i-- {
						if e.script[i].GetType() == OperationAction {
							if e.script[i].GetOperation().Op == es.Operation_Loop {

								e.blockStack.Push(block{i, true})

								break
							}
						}
					}
				}

			}

		case es.Operation_FnCall:
			{
				// NOTE: currently assuming args will be values
				gc.LogInfo("FnCall Op", op.Val, ret)
				args := []run.Value{}

				for !e.valQueue.IsEmpty() {
					val, _ := e.valQueue.PopFront()
					args = append(args, val)
				}

				fn, _ := e.stack.Get(op.Val)

				ret = fn(args...)

			}

		default:
			{
				gc.LogWarning("Token unevaluated", op.Op)
				ret = run.NilValue{}

				if !e.valQueue.IsEmpty() {
					val, _ := e.valQueue.PopFront()
					ret = e.recallIfPossible(val)
				}
			}

		}
	}

	if !e.valQueue.IsEmpty() {
		val, _ := e.valQueue.PopFront()
		ret = e.recallIfPossible(val)
	}

	return ret
}

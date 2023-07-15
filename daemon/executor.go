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
  stack  StackMap[func(...Value) Value]
	opQueue  Queue[Operation]
	valQueue Queue[Value]
}

func NewExecutor(cwd string) Executor {
  new_exec := Executor{
    stack: NewStackMap[func(...Value) Value](),
		opQueue:  NewQueue[Operation](),
		valQueue: NewQueue[Value](),
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

func (e *Executor) PushOp(op Operation) {
	e.opQueue.PushBack(op)
}

func (e *Executor) PushVal(val Value) {
	e.valQueue.PushBack(val)
}

func executeMath(op string, left, right float64) Value {
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

func valueWrap(val Value) (func(...Value) Value)  {
  return func(args ...Value) Value {
    return val
  }
}

// Will execute a full line
func (e *Executor) Exec() Value {
	var ret Value
  ret_initialized := false

  // Check if this line will exec an FNCall
  bottom := e.opQueue.PeekBottom()
  switch bottom.Op {
  case es.Operation_FnCall: {
      args := []Value{}

      for !e.valQueue.isEmpty() {
        val, _ := e.valQueue.PopFront()
        args = append(args, e.recallIfPossible(val))
      }

      fn, err := e.stack.Get(bottom.Val)

      if err != nil {
        return NilValue{}
      }

      return fn(args...)
    }
  }

  // todo : abstract out generic execution
	for !e.opQueue.isEmpty() {
    op, _ := e.opQueue.PopFront()

    switch op.Op {
  	case es.Operation_Math: {
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
    case es.Operation_Asmt: {
        if !ret_initialized {
          ret, _ = e.valQueue.PopFront()

          ret_initialized = true
        }

        e.stack.Set(op.Val, valueWrap(ret))
      }

    default: {
      gc.LogInfo("skipped", op.Op)
      ret = NilValue{}
    }

    }
	}

	return ret
}

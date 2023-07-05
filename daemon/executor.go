package main

import (
	"math"
)

type Operation int

const (
	Add Operation = iota
	Sub
	Div
	Mul
	Mod
	Pow
)

// handles right to left execution for a task
type Executor struct {
	opQueue  Queue[Operation]
	valQueue Queue[float64]
}

func NewExecutor() Executor {
	return Executor{
		opQueue:  NewQueue[Operation](),
		valQueue: NewQueue[float64](),
	}
}

func (e *Executor) PushOp(op Operation) {
	e.opQueue.PushBack(op)
}

func (e *Executor) PushVal(val float64) {
	e.valQueue.PushBack(val)
}

func executeOp(op Operation, left, right float64) float64 {
	var ret float64

	switch op {
	case Add:
		{
			ret = left + right
		}
	case Sub:
		{
			ret = left - right
		}
	case Mul:
		{
			ret = left * right
		}
	case Div:
		{
			ret = left / right
		}
	case Pow:
		{
			ret = math.Pow(left, right)
		}
	case Mod:
		{
			ret = math.Mod(left, right)
		}
	}

	return ret
}

// todo: make execution safer
// Will execute a full line
func (e *Executor) Exec() float64 {
	var ret float64

	// run the first one seperate
	op, _ := e.opQueue.PopFront()
	left, _ := e.valQueue.PopFront()
	right, _ := e.valQueue.PopFront()

	ret = executeOp(op, left, right)

	for e.opQueue.Length() != 0 {
		op, _ = e.opQueue.PopFront()
		left, _ = e.valQueue.PopFront()

		ret = executeOp(op, ret, left)
	}

	return ret
}

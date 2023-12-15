package main

import (
	es "github.com/CalebDepatie/mash/execStream"
	run "github.com/CalebDepatie/mash/runtime"
)

// handles functions that convert incoming mbyr opcodes to something a bit more useful
// mbyr = MASH Bytecode Representation

type ActionType int

const (
	OperationAction ActionType = iota
	ValueAction
)

// struct to 'simplify'...
type Action struct {
	Type ActionType
	Val  run.Value
	Op   Operation
}

func (a Action) GetType() ActionType {
	return a.Type
}

func (a Action) GetValue() run.Value {
	return a.Val
}

func (a Action) GetOperation() Operation {
	return a.Op
}

func ConvertCodes(codes *es.ExecStream) []Action {

	var actions []Action

	for _, code := range codes.ExecKeys {
		switch code.Op {
		case es.Operation_StringVal:
			{
				new_action := Action{ValueAction, run.StringValue{code.GetStringValue()}, Operation{}}
				actions = append(actions, new_action)
			}
		case es.Operation_BoolVal:
			{
				new_action := Action{ValueAction, run.BoolValue{code.GetBooleanValue()}, Operation{}}
				actions = append(actions, new_action)
			}
		case es.Operation_NumberVal:
			{
				new_action := Action{ValueAction, run.DoubleValue{code.GetNumberValue()}, Operation{}}
				actions = append(actions, new_action)
			}
		case es.Operation_RangeVal:
			{
				r := code.GetRangeValue()
				new_action := Action{ValueAction, run.RangeValue{[2]int32{r.From, r.To}}, Operation{}}
				actions = append(actions, new_action)
			}

		default:
			{
				contentStr := code.GetStringValue()
				new_action := Action{OperationAction, run.NilValue{}, Operation{code.Op, contentStr}}

				actions = append(actions, new_action)
			}
		}
	}

	return actions
}

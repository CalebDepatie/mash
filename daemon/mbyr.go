package main

import (
	es "github.com/CalebDepatie/mash/execStream"
	run "github.com/CalebDepatie/mash/runtime"
)

// handles functions that convert incoming mbyr opcodes to something a bit more useful
// mbyr = MASH Bytecode Representation

type ActionType int

const (
	If ActionType = iota
	Loop
	Value
	Op
)

type Action interface {
	GetActionType() ActionType
}

func ConvertCodes(codes *es.ExecStream) []run.Value {

	var values []run.Value

	for _, code := range codes.ExecKeys {
		switch code.Op {
		case es.Operation_StringVal:
			{
				values = append(values, run.StringValue{code.GetStringValue()})
			}
		case es.Operation_BoolVal:
			{
				values = append(values, run.BoolValue{code.GetBooleanValue()})
			}
		case es.Operation_NumberVal:
			{
				values = append(values, run.DoubleValue{code.GetNumberValue()})
			}
		case es.Operation_RangeVal:
			{
				r := code.GetRangeValue()
				values = append(values, run.RangeValue{[2]int32{r.From, r.To}})
			}
		}
	}

	return values
}

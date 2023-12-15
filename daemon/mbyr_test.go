package main

import (
	"testing"

	es "github.com/CalebDepatie/mash/execStream"
)

func TestConvertValues(t *testing.T) {
	stream := &es.ExecStream{}

	stream.ExecKeys = append(stream.ExecKeys, &es.ExecKey{
		Op:      es.Operation_StringVal,
		Content: &es.ExecKey_StringValue{StringValue: "test"},
	})

	stream.ExecKeys = append(stream.ExecKeys, &es.ExecKey{
		Op:      es.Operation_BoolVal,
		Content: &es.ExecKey_BooleanValue{BooleanValue: true},
	})

	stream.ExecKeys = append(stream.ExecKeys, &es.ExecKey{
		Op:      es.Operation_NumberVal,
		Content: &es.ExecKey_NumberValue{NumberValue: 10.0},
	})

	stream.ExecKeys = append(stream.ExecKeys, &es.ExecKey{
		Op:      es.Operation_RangeVal,
		Content: &es.ExecKey_RangeValue{RangeValue: &es.Range{From: 0, To: 10}},
	})

	embyr_formatted := ConvertCodes(stream)

	if embyr_formatted[0].String() != "test" {
		t.Error("String conversion failed")
	}

	if embyr_formatted[1].Bool() != true {
		t.Error("Bool conversion failed")
	}

	if embyr_formatted[2].Double() != 10.0 {
		t.Error("Double conversion failed")
	}

	if embyr_formatted[3].Range()[0] != 0 || embyr_formatted[3].Range()[1] != 10 {
		t.Error("Range conversion failed")
	}
}

func TestConvertBlocks(t *testing.T) {
	stream := &es.ExecStream{}

	_ = stream
}

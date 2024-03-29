package runtime

import "strconv"

type ValueType int

const (
	String ValueType = iota
	Iden
	Nil
	Double
	Bool
	Range
)

type Value interface {
	Type() ValueType
	String() string
	Double() float64
	Bool() bool
	Range() [2]int32
}

// --- NilValue ---
// provides a stand in for "nothing"

type NilValue struct{}

func (v NilValue) Type() ValueType {
	return Nil
}

func (v NilValue) String() string {
	return "nil"
}

func (v NilValue) Double() float64 {
	return 0
}

func (v NilValue) Bool() bool {
	return false
}

func (v NilValue) Range() [2]int32 {
	return [2]int32{0, 0}
}

// --- StringValue ---

type StringValue struct {
	Val string
}

func (v StringValue) Type() ValueType {
	return String
}

func (v StringValue) String() string {
	return v.Val
}

func (v StringValue) Double() float64 {
	return 0
}

func (v StringValue) Bool() bool {
	return false
}

func (v StringValue) Range() [2]int32 {
	return [2]int32{0, 0}
}

// --- IdenValue ---

type IdenValue struct {
	Val string
}

func (v IdenValue) Type() ValueType {
	return Iden
}

func (v IdenValue) String() string {
	return v.Val
}

func (v IdenValue) Double() float64 {
	return 0
}

func (v IdenValue) Bool() bool {
	return false
}

func (v IdenValue) Range() [2]int32 {
	return [2]int32{0, 0}
}

// --- DoubleValue ---

type DoubleValue struct {
	Val float64
}

func (v DoubleValue) Type() ValueType {
	return Double
}

func (v DoubleValue) String() string {
	return strconv.FormatFloat(v.Val, 'f', -1, 64)
}

func (v DoubleValue) Double() float64 {
	return v.Val
}

func (v DoubleValue) Bool() bool {
	return false
}

func (v DoubleValue) Range() [2]int32 {
	return [2]int32{0, 0}
}

// --- BoolValue ---

type BoolValue struct {
	Val bool
}

func (v BoolValue) Type() ValueType {
	return Bool
}

func (v BoolValue) String() string {
	return strconv.FormatBool(v.Val)
}

func (v BoolValue) Double() float64 {
	return 0
}

func (v BoolValue) Bool() bool {
	return v.Val
}

func (v BoolValue) Range() [2]int32 {
	return [2]int32{0, 0}
}

// --- RangeValue ---

type RangeValue struct {
	Val [2]int32
}

func (v RangeValue) Type() ValueType {
	return Range
}

func (v RangeValue) String() string {
	return strconv.Itoa(int(v.Val[0])) + ":" + strconv.Itoa(int(v.Val[1]))
}

func (v RangeValue) Double() float64 {
	return 0
}

func (v RangeValue) Bool() bool {
	return false
}

func (v RangeValue) Range() [2]int32 {
	return v.Val
}

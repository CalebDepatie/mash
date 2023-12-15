package runtime

import "errors"

type Queue[T any] struct {
	array     []T
	cur_start int
}

func NewQueue[T any]() Queue[T] {
	new_queue := Queue[T]{
		array:     make([]T, 0),
		cur_start: 0,
	}

	return new_queue
}

func (q *Queue[T]) PushBack(Value T) {
	q.array = append(q.array, Value)
}

func (q *Queue[T]) PopFront() (T, error) {
	if len(q.array) == q.cur_start {
		var def T
		return def, errors.New("Attempt to pop from empty queue")
	}

	ret := q.array[q.cur_start]
	q.cur_start++

	return ret, nil
}

func (q Queue[T]) Length() int {
	return len(q.array) - q.cur_start
}

func (q Queue[T]) PeekBottom() T {
	return q.array[q.cur_start]
}

func (q Queue[T]) Remaining() []T {
	return q.array[q.cur_start:len(q.array)]
}

func (q Queue[T]) isEmpty() bool {
	if len(q.array) == q.cur_start {
		return true
	} else {
		return false
	}
}

func (q Queue[T]) Index() int {
	return q.cur_start
}

func (q *Queue[T]) Clear() {
	q.array = make([]T, 0)
	q.cur_start = 0
}

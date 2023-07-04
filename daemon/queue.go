package main

import "errors"

type Queue[T any] struct {
  array []T
}

func NewQueue[T any]() Queue[T] {
  new_queue := Queue[T]{
    array: make([]T, 0),
  }

  return new_queue
}

func (q *Queue[T]) PushBack(Value T) {
  q.array = append(q.array, Value)
}

func (q *Queue[T]) PopFront() (T, error) {
  if len(q.array) == 0 {
    var def T
    return def, errors.New("Attempt to pop from empty queue")
  }

  ret := q.array[0]
  q.array = q.array[1:]

  return ret, nil
}

func (q Queue[T]) Length() int {
  return len(q.array)
}

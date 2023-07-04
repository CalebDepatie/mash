package main

import "testing"

func TestQueueInsert(t *testing.T) {
  value := "test!"

  q := NewQueue[string]()

  q.PushBack(value)

  ret, _ := q.PopFront()

  if ret != value {
    t.Errorf("Queue in/out incorrect\nExpecting: '%s' Received: '%s'", value, ret)
  }
}

func TestEmptyError(t *testing.T) {
  q := NewQueue[string]()

  _, err := q.PopFront()

  if err == nil {
    t.Errorf("Error was not return for retreiving from empty queue")
  }
}

func TestLen(t *testing.T) {
  value := "test!"

  q := NewQueue[string]()

  q.PushBack(value)

  length := q.Length()

  if length != 1 {
    t.Errorf("Incorrect length returned\nExpecting: 1, Recieved: %d", length)
  }
}

package main

import "testing"

func TestStack_IsEmpty(t *testing.T) {
	s := Stack[int]{}

	if !s.IsEmpty() {
		t.Errorf("IsEmpty() on empty stack should return true")
	}
}

func TestStack_Peek(t *testing.T) {
	s := Stack[int]{}

	s.Push(1)
	s.Push(2)
	s.Push(3)
	if s.Peek() != 3 {
		t.Errorf("Peek() should return 3")
	}
}

func TestStack_Pop(t *testing.T) {
	s := Stack[int]{}

	s.Push(1)
	s.Push(2)
	s.Push(3)
	if s.Pop() != 3 {
		t.Errorf("Pop() should return 3")
	}
	if s.Pop() != 2 {
		t.Errorf("Pop() should return 2")
	}
	if s.Pop() != 1 {
		t.Errorf("Pop() should return 1")
	}
}

func TestStack_IsEmptyAfterPop(t *testing.T) {
	s := Stack[int]{}

	s.Push(1)
	s.Pop()
	if !s.IsEmpty() {
		t.Errorf("IsEmpty() on empty stack should return true")
	}
}

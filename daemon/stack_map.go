package main

import "errors"

type StackMap struct {
  curMap map[string]string // todo: using strings for everything will not be efficient
  prevStack *StackMap
}

func NewStackMap() StackMap {
  new_stack := StackMap{
    curMap: make(map[string]string),
    prevStack: nil,
  }

  return new_stack
}

func (s *StackMap) Set(key, val string) {
  s.curMap[key] = val
}

func (s *StackMap) Get(key string) (string, error) {
  val, ok := s.curMap[key]

  if ok {
    return val, nil
  }

  if s.prevStack != nil {
    return s.prevStack.Get(key)
  }

  return "", errors.New("Key does not exist")
}

func (s *StackMap) NewLayer() {
  new_stack := NewStackMap()
  new_stack.prevStack = s
}

func (s *StackMap) PopLayer() error {
  if s.prevStack == nil {
    return errors.New("No additional layer to pop")
  }

  // move back up the stack
  s = s.prevStack
  return nil
}

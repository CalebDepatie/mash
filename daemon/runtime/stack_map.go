package runtime

import "errors"

type StackMap[T any] struct {
	curMap    map[string]T
	prevStack *StackMap[T]
}

func NewStackMap[T any]() StackMap[T] {
	new_stack := StackMap[T]{
		curMap:    make(map[string]T),
		prevStack: nil,
	}

	return new_stack
}

func (s *StackMap[T]) Set(key string, val T) {

	// if the key exists, update the value in the appropriate layer
	check := s.prevStack
	for check != nil {
		_, ok := s.curMap[key]

		if !ok {
			check.curMap[key] = val
		}

		check = check.prevStack
	}

	// TODO: this makes initializing a variable slower than it needs to be
	s.curMap[key] = val
}

func (s *StackMap[T]) Get(key string) (T, error) {
	val, ok := s.curMap[key]

	if ok {
		return val, nil
	}

	if s.prevStack != nil {
		return s.prevStack.Get(key)
	}

	var def T
	return def, errors.New("Key does not exist")
}

func (s *StackMap[T]) NewLayer() {
	new_stack := NewStackMap[T]()
	new_stack.prevStack = s
}

func (s *StackMap[T]) PopLayer() error {
	if s.prevStack == nil {
		return errors.New("No additional layer to pop")
	}

	// move back up the stack
	s = s.prevStack
	return nil
}

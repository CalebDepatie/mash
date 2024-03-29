package runtime

import "testing"

func TestSingleLevelMap(t *testing.T) {
	stack := NewStackMap[string]()

	key, value := "test key", "test value"

	stack.Set(key, value)

	ret, _ := stack.Get(key)

	if ret != value {
		t.Errorf("Map returned incorrect value\nExpected: '%s', Recieved '%s'", value, ret)
	}
}

func TestBasicErrors(t *testing.T) {
	stack := NewStackMap[string]()

	_, err := stack.Get("test")

	if err == nil {
		t.Errorf("Expected error but nil was recieved")
	}

	err = stack.PopLayer()

	if err == nil {
		t.Errorf("Expected error but nil was recieved")
	}
}

func TestMultiLevel(t *testing.T) {
	stack := NewStackMap[string]()

	key, value := "test key", "test value"
	stack.Set(key, value)

	stack.NewLayer()

	key2, value2 := "test key2", "test value2"
	stack.Set(key2, value2)

	ret, _ := stack.Get(key)
	if ret != value {
		t.Errorf("Map returned incorrect value\nExpected: '%s', Recieved '%s'", value, ret)
	}

	ret, _ = stack.Get(key2)
	if ret != value2 {
		t.Errorf("Map returned incorrect value\nExpected: '%s', Recieved '%s'", value2, ret)
	}

	// ensuring setting a key that already exists is propagated down the layers
	key, value = "test key", "test value changed"
	stack.Set(key, value)

	stack.PopLayer()

	ret, _ = stack.Get(key)
	if ret != value {
		t.Errorf("Map returned incorrect value\nExpected: '%s', Recieved '%s'", value, ret)
	}
}

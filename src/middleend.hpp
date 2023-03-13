#pragma once

#include <string>

enum Operation {
  Begin,
  End,
  ScopeStart,
  ScopeEnd,
  Math,
  Loop,
  Cond,
  Run,
};

struct Execution_Key {
  Operation op;
  std::string value;
};
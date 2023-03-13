#pragma once

#include <string>

enum Operation {
  Begin,  // Start of a new script, gives the CWD
  End,    // Tells the executor it can safely end the task
  ScopeStart,
  ScopeEnd,
  Math,
  Asmt,
  Loop,
  Cond,
  Cmd,
  Notify,
  Accept,
  ClearReg,  // essentially an End Line
};

struct Execution_Key {
  Operation   op;
  std::string value;
};
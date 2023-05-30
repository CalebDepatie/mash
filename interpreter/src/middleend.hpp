#pragma once

#include <string>
#include <vector>
#include <memory>

#include "frontend/parser.hpp"

#define OP_STRING(op)                                                                          \
  { op, #op }

enum Operation {
  Begin,  // Start of a new script, gives the CWD
  End,    // Tells the executor it can safely end the task
  ScopeStart,
  ScopeEnd,
  Math,
  Asmt,
  Loop,
  Cond,
  Recall,
  Notify,
  Accept,
  ClearReg,  // essentially an End Line
};

inline std::unordered_map<Operation, std::string> op_names = {
  OP_STRING(Operation::Begin),
  OP_STRING(Operation::End),
  OP_STRING(Operation::ScopeStart),
  OP_STRING(Operation::ScopeEnd),
  OP_STRING(Operation::Math),
  OP_STRING(Operation::Asmt),
  OP_STRING(Operation::Loop),
  OP_STRING(Operation::Cond),
  OP_STRING(Operation::Recall),
  OP_STRING(Operation::Notify),
  OP_STRING(Operation::Accept),
  OP_STRING(Operation::ClearReg),
};

#undef OP_STRING

struct Execution_Key {
  Operation   op;
  std::string value;

  auto toString() -> std::string;
};

auto bakeAST(std::shared_ptr<parser::ASTNode> execTop) -> std::vector<Execution_Key>;

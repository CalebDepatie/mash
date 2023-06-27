#include "ast.hpp"
#include "tokens.hpp"
#include <iostream>

constexpr int INDENT_SIZE = 2;

namespace parser {

auto createIndent(const int& depth) -> std::string {
  return std::string(depth * INDENT_SIZE, ' ');
}

auto stringNext(std::shared_ptr<Node> n, const int depth, const bool newline) -> std::string {
  if (n) {
    return n->toString(depth, newline);
  } else {
    return "";
  }
}

// --- Generic Node ---
Node::Node() : next(nullptr) {}

Node::Node(std::shared_ptr<Node> child) : next(child) {}

auto Node::toString(int depth, bool newline) -> std::string {
  std::string indent = "";
  if (newline) indent += "\n";

  indent += createIndent(depth);
  if (this->next != nullptr)
    return indent + "Node" + this->next->toString(++depth, true);

  return indent + "Node";
}

// --- Math ---
auto Math::toString(int depth, bool newline) -> std::string {
  std::string s = "";
  if (newline) s += "\n";
  s += createIndent(depth+2) + "Math" + "\n";

  auto stringMath = [](std::variant<std::shared_ptr<Number>, std::shared_ptr<Math>> n, int d) {
    if (std::holds_alternative<std::shared_ptr<Number>>(n)) {
      return std::get<std::shared_ptr<Number>>(n)->toString(d, false);

    } else if (std::holds_alternative<std::shared_ptr<Math>>(n)) {
      return std::get<std::shared_ptr<Math>>(n)->toString(d, false);
    } else {
      return std::string("");
    }
  };

  s += stringMath(this->left, depth) + token::tkn_names[this->operation] + stringMath(this->right, depth+4);
  s += stringNext(this->next, ++depth, true);

  return s;
}

// --- Conditional ---
auto Conditional::toString(int depth, bool newline) -> std::string {\
  std::string s = "";
  if (newline) s += "\n";
  s += createIndent(depth+2) + "Cond" + "\n";

  s += this->left->toString(depth, false) + token::tkn_names[this->operation] + this->right->toString(depth+4, false);
  s += stringNext(this->next, ++depth, true);

  return s;
}

// --- Scope ---
auto Scope::toString(int depth, bool newline) -> std::string {
  std::string s = "";
  if (newline) s += "\n";
  s += createIndent(depth) + "Scope: " + "\n";

  s += stringNext(this->line_top, depth, true);

  s += stringNext(this->next, ++depth, true);

  return s;
}

// --- IfCond ---
auto IfCond::toString(int depth, bool newline) -> std::string {
  std::string s = "";
  if (newline) s += "\n";
  s += createIndent(depth) + "If: ";

  s += this->cond->toString(depth, false);
  s += this->scope->toString(depth+2, true);

  s += stringNext(this->next, ++depth, true);

  return s;
}

// --- FnDef ---
auto FnDef::toString(int depth, bool newline) -> std::string {
  std::string s = "";
  if (newline) s += "\n";
  s += createIndent(depth) + "FnDef: ";

  for (auto arg : this->arg_names) {
    s += arg + ", ";
  }

  s += this->scope->toString(depth+2, true);

  s += stringNext(this->next, ++depth, true);

  return s;
}

// --- FnCall ---
auto FnCall::toString(int depth, bool newline) -> std::string {
  std::string s = "";
  if (newline) s += "\n";
  s += createIndent(depth) + "FnCall: ";
  s += this->iden + "\n";

  for (auto arg : this->args) {
    if (std::holds_alternative<std::shared_ptr<Value>>(arg)) {
      s += std::get<std::shared_ptr<Value>>(arg)->toString(depth, false);

    } else if (std::holds_alternative<std::shared_ptr<Scope>>(arg)) {
      s += std::get<std::shared_ptr<Scope>>(arg)->toString(depth, false);
    }
  }

  s += stringNext(this->next, ++depth, true);

  return s;
}

// --- Asmt ---
auto Asmt::toString(int depth, bool newline) -> std::string {
  std::string s = "";
  if (newline) s += "\n";
  s += createIndent(depth) + "Asmt: ";
  s += this->iden + "\n";

  if (std::holds_alternative<std::shared_ptr<Value>>(this->val)) {
    s += std::get<std::shared_ptr<Value>>(this->val)->toString(depth, false);

  } else if (std::holds_alternative<std::shared_ptr<FnDef>>(this->val)) {
    s += std::get<std::shared_ptr<FnDef>>(this->val)->toString(depth, false);
  }

  s += stringNext(this->next, ++depth, true);

  return s;
}

// --- Values ---
auto Number::toString(int depth, bool newline) -> std::string {
  std::string s = "";
  if (newline) s += "\n";

  s += createIndent(depth) + "Num: " + std::to_string(this->val);

  s += stringNext(this->next, ++depth, true);

  return s;
}

Number::Number(std::string s) {
  this->val = std::stod(s);
}

auto Boolean::toString(int depth, bool newline) -> std::string {
  std::string s = "";
  if (newline) s += "\n";

  s += createIndent(depth) + "Bool: " + std::to_string(this->val);

  s += stringNext(this->next, ++depth, true);

  return s;
}

Boolean::Boolean(std::string s) {
  if (s == "true") {
    this->val = true;

  } else if (s == "false") {
    this->val = false;
  }
}

auto String::toString(int depth, bool newline) -> std::string {
  std::string s = "";
  if (newline) s += "\n";

  s += createIndent(depth) + "String: " + this->val;

  s += stringNext(this->next, ++depth, true);

  return s;
}

String::String(std::string s) {
  this->val = s;
}

auto NamedVal::toString(int depth, bool newline) -> std::string {
  std::string s = "";
  if (newline) s += "\n";

  s += createIndent(depth) + "Var: " + this->iden;

  s += stringNext(this->next, ++depth, true);

  return s;
}

NamedVal::NamedVal(std::string s) {
  this->iden = s;
}
}

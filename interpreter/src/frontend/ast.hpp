#pragma once

#include "tokens.hpp"

#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace parser {
struct Node {
  std::shared_ptr<Node> next;

  virtual auto toString(int depth = 0, bool newline = true) -> std::string;
  Node();
  Node(std::shared_ptr<Node> child);
  virtual ~Node() {};
};

struct Value : Node {
  virtual auto toString(int depth, bool newline) -> std::string = 0;
  virtual ~Value() {};
};

struct Number : public Value {
  double val;

  auto toString(int depth, bool newline) -> std::string;
  Number(std::string s);
  ~Number(){};
};

struct String : public Value {
  std::string val;

  auto toString(int depth, bool newline) -> std::string;
  String(std::string s);
  ~String(){};
};

struct Boolean : public Value {
  bool val;

  auto toString(int depth, bool newline) -> std::string;
  Boolean(std::string s);
  ~Boolean(){};
};

struct NamedVal : public Value {
  std::string iden;

  auto toString(int depth, bool newline) -> std::string;
  NamedVal(std::string s);
  ~NamedVal(){};
};

struct Math : public Value {
  token::tkn_type operation;
  std::variant<std::shared_ptr<Number>, std::shared_ptr<NamedVal>, std::shared_ptr<Math>> left;
  std::variant<std::shared_ptr<Number>, std::shared_ptr<NamedVal>, std::shared_ptr<Math>> right;

  auto toString(int depth, bool newline) -> std::string;
};

struct Conditional : public Value {
  token::tkn_type operation;
  std::shared_ptr<Value> left;
  std::shared_ptr<Value> right;

  auto toString(int depth, bool newline) -> std::string;
};

struct Scope : public Node {
  std::shared_ptr<Node> line_top;

  auto toString(int depth = 0, bool newline = true) -> std::string;
};

struct IfCond : public Node {
  std::shared_ptr<Conditional> cond;
  std::shared_ptr<Scope> scope;

  auto toString(int depth, bool newline) -> std::string;
};

struct FnDef : public Node {
  std::vector<std::string> arg_names;
  std::shared_ptr<Scope> scope;

  auto toString(int depth, bool newline) -> std::string;
};

struct FnCall : public Value {
  std::string iden;
  std::vector<std::variant<std::shared_ptr<Value>, std::shared_ptr<Scope>>> args;

  auto toString(int depth, bool newline) -> std::string;
};

struct Asmt : public Node {
  std::string iden;
  std::variant<std::shared_ptr<Value>, std::shared_ptr<FnDef>> val;

  auto toString(int depth, bool newline) -> std::string;
};

struct Loop : public Node {
  std::shared_ptr<Asmt> asmt;
  std::shared_ptr<Scope> scope;

  auto toString(int depth, bool newline) -> std::string;
};

struct Range : public Value {
  std::shared_ptr<Number> from;
  std::shared_ptr<Number> to;

  auto toString(int depth, bool newline) -> std::string;
};
}

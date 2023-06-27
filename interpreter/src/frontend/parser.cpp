#include "parser.hpp"
#include "../common.hpp"
#include <unordered_map>
#include <utility>
#include <iostream>

constexpr int INDENT_SIZE = 2;

namespace parser {
// helper function checking if the token is an operation
auto check_op(token::Token tkn) -> bool {
  using namespace token;
  return tkn.type == tkn_type::Op_add || tkn.type == tkn_type::Op_sub ||
         tkn.type == tkn_type::Op_mult || tkn.type == tkn_type::Op_div ||
         tkn.type == tkn_type::Op_mod || tkn.type == tkn_type::Op_pow;
}

// todo: add remaining value options
auto check_value(std::vector<token::Token>& tokens, int& pc) -> bool {
  return tokens[pc].type == token::tkn_type::Num
      || tokens[pc].type == token::tkn_type::String
      || tokens[pc].type == token::tkn_type::Iden
      || tokens[pc].type == token::tkn_type::Bool;
}

auto parseScope(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Scope>;
auto parseValue(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Value>;

auto parseFnCall(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<FnCall> {
  auto fncall_node = std::make_shared<FnCall>(FnCall());

  fncall_node->iden = tokens[pc].value;
  pc++;

  while (tokens[pc].type != token::tkn_type::End) {
    if (tokens[pc].type == token::tkn_type::Scope_start) {
      fncall_node->args.emplace_back(parseScope(tokens, pc));

    } else if (check_value(tokens, pc)) {
      fncall_node->args.emplace_back(parseValue(tokens, pc));
    }

    pc++;
  }

  return fncall_node;
}

auto parseScope(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Scope> {
  auto scope_node = std::make_shared<Scope>(Scope());

  return scope_node;
}

auto parseValue(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Value> {
  std::shared_ptr<Value> value_node;

  switch (tokens[pc].type) {
    case token::tkn_type::Num: {
      value_node = std::make_shared<Number>(Number(tokens[pc].value));

      break;
    }
    case token::tkn_type::String: {
      value_node = std::make_shared<String>(String(tokens[pc].value));

      break;
    }
    case token::tkn_type::Iden: {
      value_node = std::make_shared<NamedVal>(NamedVal(tokens[pc].value));

      break;
    }
    case token::tkn_type::Bool: {
      value_node = std::make_shared<Boolean>(Boolean(tokens[pc].value));

      break;
    }
    default: {
      print_error("Parsing value and expect string, number, or iden. Recieved: " +
        token::tkn_names[tokens[pc].type]);
      break;
    }
  }

  return value_node;
}

auto parseFnDef(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<FnDef> {
  auto fndef_node = std::make_shared<FnDef>(FnDef());

  return fndef_node;
}

auto parseAsmt(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Asmt> {
  auto asmt_node = std::make_shared<Asmt>(Asmt());

  // first tkn is iden
  asmt_node->iden = tokens[pc].value;
  pc += 2; // second is ->

  // third is a value or scope
  if (check_value(tokens, pc)) {
    asmt_node->val = parseValue(tokens, pc);

  } else if (tokens[pc].type == token::tkn_type::FuncDef) {
    asmt_node->val = parseFnDef(tokens, pc);
  }

  return asmt_node;
}

// generates a parse tree / AST from the tokens
auto parse(std::vector<token::Token>& tokens) -> std::shared_ptr<Node> {
  using namespace token;

  std::shared_ptr<Node> ast(new Node());
  std::shared_ptr<Node> bottom = ast;
  int                   pc     = 0;  // program / parse counter

  do {
    if (tokens[pc].type == tkn_type::Iden && tokens[pc+1].type == tkn_type::Assignment) {
      bottom->next = parseAsmt(tokens, pc);
      pc++;

    } else if (tokens[pc].type == tkn_type::Iden) {
      // if all other options are exhausted, this must be a function call
      bottom->next = parseFnCall(tokens, pc);
      pc++;

    } if (tokens[pc].type == tkn_type::End) {
      // empty line! making the assumption that whole lines are covered in previous cases
    }

    // valid syntax should be covered by previous cases
    if (bottom->next)
      bottom = bottom->next;
    else
      pc++;

  } while (pc < tokens.size());

  return ast;
}

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

}  // namespace parser

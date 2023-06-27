#include "parser.hpp"

#include "tokens.hpp"
#include "ast.hpp"
#include "../common.hpp"
#include <unordered_map>
#include <utility>
#include <iostream>

namespace parser {
// declarations
auto parseFnCall(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<FnCall>;
auto parseScope(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Scope>;
auto parseValue(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Value>;
auto parseFnDef(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<FnDef>;
auto parseAsmt(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Asmt>;
auto parseIfCond(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<IfCond>;
auto parseMath(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Math>;
// auto parseLoop(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Loop>;
auto parseCond(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Conditional>;

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

}  // namespace parser
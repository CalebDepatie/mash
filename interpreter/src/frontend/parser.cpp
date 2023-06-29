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
auto parseValue(std::vector<token::Token>& tokens, int& pc, const bool composite=true, const bool fncheck=true) -> std::shared_ptr<Value>;
auto parseFnDef(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<FnDef>;
auto parseAsmt(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Asmt>;
auto parseIfCond(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<IfCond>;
auto parseMath(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Math>;
auto parseLoop(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Loop>;
auto parseCond(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Conditional>;
auto parseRange(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Range>;

// helper function checking if the token is an operation
auto check_op(const token::Token tkn) -> bool {
  using namespace token;
  return tkn.type == tkn_type::Op_add || tkn.type == tkn_type::Op_sub ||
         tkn.type == tkn_type::Op_mult || tkn.type == tkn_type::Op_div ||
         tkn.type == tkn_type::Op_mod || tkn.type == tkn_type::Op_pow;
}

auto check_cond(const token::Token tkn) -> bool {
  using namespace token;
  return tkn.type == tkn_type::Op_lt || tkn.type == tkn_type::Op_lte ||
         tkn.type == tkn_type::Op_gt || tkn.type == tkn_type::Op_gte ||
         tkn.type == tkn_type::Op_eq || tkn.type == tkn_type::Op_ne;
}

// todo: add remaining value options
auto check_value(const token::Token tkn) -> bool {
  using namespace token;
  return tkn.type == tkn_type::Num
      || tkn.type == tkn_type::String
      || tkn.type == tkn_type::Iden
      || tkn.type == tkn_type::Bool;
}

auto parseFnCall(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<FnCall> {
  auto fncall_node = std::make_shared<FnCall>(FnCall());

  fncall_node->iden = tokens[pc].value;
  ++pc;

  while (tokens[pc].type != token::tkn_type::End) {
    if (tokens[pc].type == token::tkn_type::Scope_start) {
      ++pc;
      fncall_node->args.push_back(parse(tokens, pc));

    } else if (check_value(tokens[pc])) {
      fncall_node->args.push_back(parseValue(tokens, pc, true, false));
    }

    ++pc;
  }

  return fncall_node;
}

auto parseMath(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Math> {
  auto math_node = std::make_shared<Math>(Math());

  // (identifier/math/number) SP operation SP (identifier/math/number)
  if (tokens[pc].type == token::tkn_type::Num) {
    math_node->left = std::make_shared<Number>(Number(tokens[pc].value));

  } else if (tokens[pc].type == token::tkn_type::Iden) {
    math_node->left = std::make_shared<NamedVal>(NamedVal(tokens[pc].value));
  }

  pc++;
  math_node->operation = tokens[pc++].type;

  if (pc+2 < tokens.size() && tokens[pc+1].type != token::tkn_type::End) { // safety for larger requirements
    if (check_op(tokens[pc+1])) {
      math_node->right = parseMath(tokens, pc);
    }

  } else if (tokens[pc].type == token::tkn_type::Num) {
    math_node->right = std::make_shared<Number>(Number(tokens[pc].value));

  } else if (tokens[pc].type == token::tkn_type::Iden) {
    math_node->right = std::make_shared<NamedVal>(NamedVal(tokens[pc].value));
  }

  return math_node;
}

auto parseValue(std::vector<token::Token>& tokens, int& pc, const bool composite, const bool fncheck) -> std::shared_ptr<Value> {
  using namespace token;

  std::shared_ptr<Value> value_node;

  if (composite) {
    if (pc+2 < tokens.size()) { // safety for larger requirements
      if (check_op(tokens[pc+1])) {
        value_node = parseMath(tokens, pc);

      } else if (check_cond(tokens[pc+1])) {
        value_node = parseCond(tokens, pc);

      } else if (tokens[pc+1].type == tkn_type::Range) {
        value_node = parseRange(tokens, pc);
      }
    }

    if (value_node) {
      return value_node;

    } else if (pc+1 < tokens.size() && fncheck) {
      if (tokens[pc].type == tkn_type::Iden) {
        value_node = parseFnCall(tokens, pc);
        return value_node;
      }
    }
  }

  if (tokens[pc].type == tkn_type::Num) {
    value_node = std::make_shared<Number>(Number(tokens[pc].value));

  } else if (tokens[pc].type == tkn_type::String) {
    value_node = std::make_shared<String>(String(tokens[pc].value));

  } else if (tokens[pc].type == tkn_type::Iden) {
    value_node = std::make_shared<NamedVal>(NamedVal(tokens[pc].value));

  } else if (tokens[pc].type == tkn_type::Bool) {
    value_node = std::make_shared<Boolean>(Boolean(tokens[pc].value));

  } else {
    print_error("Parsing value and expect string, number, or iden. Recieved: " +
      token::tkn_names[tokens[pc].type]);
  }

  return value_node;
}

auto parseRange(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Range> {
  auto range_node = std::make_shared<Range>(Range());

  range_node->from = std::make_shared<Number>(Number(tokens[pc].value));
  pc += 2;
  range_node->to = std::make_shared<Number>(Number(tokens[pc].value));

  return range_node;
}

auto parseFnDef(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<FnDef> {
  auto fndef_node = std::make_shared<FnDef>(FnDef());

  // "fn" SP "(" *identifier ")" SP scope
  pc+=2;

  while (tokens[pc].type != token::tkn_type::Param_list_end) {
    fndef_node->arg_names.emplace_back(tokens[pc].value);
    ++pc;
  }
  ++pc;

  fndef_node->scope = parse(tokens, pc);

  return fndef_node;
}

auto parseLoop(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Loop> {
  auto loop_node = std::make_shared<Loop>(Loop());

  // "for" SP assignment SP scope
  ++pc;

  loop_node->asmt = parseAsmt(tokens, pc);
  ++pc;

  // this is to counteract an issue from ambiguous syntax
  // when a function call is used for the loop asmt
  if (tokens[pc].type == token::tkn_type::Scope_start) {
    ++pc;
    loop_node->scope = parse(tokens, pc);

  } else {
    // assumes the last 'argument' is the loop scope
    if (auto value_asmt = std::get_if<std::shared_ptr<Value>>(&loop_node->asmt->val)) {
      if (auto fncall = std::dynamic_pointer_cast<FnCall>(*value_asmt)) {
        if (auto scope_arg = std::get_if<std::shared_ptr<Scope>>(&fncall->args[fncall->args.size()-1])) {
          loop_node->scope = *scope_arg;
          fncall->args.pop_back();

        }
      }
    }
  }

  return loop_node;
}

auto parseCond(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Conditional> {
  auto cond_node = std::make_shared<Conditional>(Conditional());

  // value SP comparator SP value
  cond_node->left = parseValue(tokens, pc, false);
  ++pc;

  cond_node->operation = tokens[pc].type;
  ++pc;

  cond_node->right = parseValue(tokens, pc, true);

  return cond_node;
}

auto parseIfCond(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<IfCond> {
  auto if_node = std::make_shared<IfCond>(IfCond());

  //"if" SP comparator SP scope
  ++pc;

  if_node->cond = parseCond(tokens, pc);
  ++pc;

  if_node->scope = parse(tokens, pc);

  return if_node;
}

auto parseAsmt(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Asmt> {
  auto asmt_node = std::make_shared<Asmt>(Asmt());

  // first tkn is iden
  asmt_node->iden = tokens[pc].value;
  pc += 2; // second is ->

  // third is a value or scope
  if (check_value(tokens[pc])) {
    asmt_node->val = parseValue(tokens, pc);

  } else if (tokens[pc].type == token::tkn_type::FuncDef) {
    asmt_node->val = parseFnDef(tokens, pc);
  }

  return asmt_node;
}

// generates a parse tree / AST from the tokens
auto parse(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Scope> {
  using namespace token;

  auto ast = std::make_shared<Scope>(Scope());
  ast->line_top = std::make_shared<Node>(Node()); // placeholder node

  std::shared_ptr<Node> bottom = ast->line_top;

  do {
    if (tokens[pc].type == tkn_type::Iden && tokens[pc+1].type == tkn_type::Assignment) {
      bottom->next = parseAsmt(tokens, pc);
      pc++;

    } else if (tokens[pc].type == tkn_type::Cond) {
      bottom->next = parseIfCond(tokens, pc);
      pc++;

    } else if (tokens[pc].type == tkn_type::For) {
      bottom->next = parseLoop(tokens, pc);
      pc++;

    } else if (tokens[pc].type == tkn_type::Iden) {
      // if all other options are exhausted, this must be a function call
      bottom->next = parseFnCall(tokens, pc);
      pc++;

    } else if (tokens[pc].type == tkn_type::Scope_end) {
      pc++;
      break;

    } else if (tokens[pc].type == tkn_type::End) {
      // empty line! making the assumption that whole lines are covered in previous cases
    }

    // valid syntax should be covered by previous cases
    if (bottom->next)
      bottom = bottom->next;
    else
      pc++;

  } while (pc < tokens.size());

  // remove place holder node
  ast->line_top = ast->line_top->next;

  return ast;
}

}  // namespace parser

#include "lexer.h"
#include "tokens.hpp"
#include "builtins.h"

auto lexer::lex(std::vector<std::string> args) -> std::vector<token::Token> {
  using namespace token;

  std::vector<Token> tokens;

  // branchy lexing, I'd love to use a switch BUT c++
  do {
    auto val = pop_front(args);

    if (builtins.count(val)) {tokens.emplace_back(Token(token_type::Reserved, val)); continue;}
    if (check_if_op(val)) {tokens.emplace_back(Token(token_type::Operator, val)); continue;}
    if (val == "<-" || val == "=") {tokens.emplace_back(Token(token_type::Assignment, val)); continue;}
    if (val == "\n" || val == "\r\n") {tokens.emplace_back(Token(token_type::End, val)); continue;}

    // default case
    tokens.emplace_back(Token(token_type::Value, val));
  } while (args.size() != 0);

  return tokens;
}

// checks if a string is an operation
auto lexer::check_if_op(std::string arg) -> bool {
  return arg == "+"
      || arg == "-"
      || arg == "*"
      || arg == "^"
      || arg == "%";
}

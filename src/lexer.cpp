#include "lexer.h"
#include "builtins.h"

auto lexer::lex(std::queue<std::string> args) -> std::queue<token::Token> {
  using namespace token;

  std::queue<Token> tokens;

  // branchy lexing, I'd love to use a switch BUT c++
  do {
    auto val = pop_front(args);

    if (builtins.count(val)) {tokens.push(Token(token_type::Reserved, val)); continue;}

    // default case
    tokens.push(Token(token_type::Identifier, val));
  } while (args.size() != 0);

  return tokens;
}

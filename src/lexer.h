#pragma once

namespace token {
  class Token;
}

namespace lexer {
  auto lex(std::vector<std::string> args) -> std::vector<token::Token>;
  inline auto check_if_op(std::string arg) -> bool;
}

#pragma once

namespace token {
  class Token;
}

namespace lexer {
  auto lex(std::vector<std::string> args) -> std::vector<token::Token>;
  inline auto check_if_op(std::string arg) -> bool;

  // not sure if a faster / more efficient way to do this exists ?
  inline std::unordered_map<std::string, int, imaphash, imapequal> identifiers;
}

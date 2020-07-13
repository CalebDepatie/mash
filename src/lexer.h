#pragma once

namespace token {
  class Token;
}

namespace lexer {
  auto lex(std::string args) -> std::vector<token::Token>;
  auto lexlet(std::string s) -> token::Token;
  auto check(char c) -> bool;
  auto check_if_num(std::string s) -> bool;
  auto check_if_string(std::string s) -> bool;

  // not sure if a faster / more efficient way to do this exists ?
  inline std::unordered_map<std::string, int, imaphash, imapequal> identifiers;
}

#pragma once

namespace token {
  enum class token_type {
    Reserved,
    Identifier,
    Operator,
    End,
  };

  struct Token {
    token_type type;
    std::string value;

    explicit Token(token_type type, std::string value)
      : type(type), value(value) {};
  };
}

namespace lexer {
  auto lex(std::queue<std::string> args) -> std::queue<token::Token>;
}

#pragma once

namespace token {
  enum class token_type {
    Reserved,
    Identifier,
    Value,
    Operator,
    Assignment,
    End,
  };

  struct Token {
    token_type type;
    std::string value;

    explicit Token(token_type type, std::string value)
      : type(type), value(value) {};
  };
}

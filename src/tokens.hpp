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


    Token() {this->value="";};
    Token(token_type type, std::string value)
      : type(type), value(value) {};

    inline auto toString() -> std::string {
      return "[ " + std::to_string(static_cast<int>(this->type)) + ": " + this->value + " ]"; // no elegant way to convert enum to string
    }
  };
}

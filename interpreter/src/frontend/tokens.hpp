#pragma once

#include <string>
#include <unordered_map>

#define TOKEN_STRING(tkn)                                                                          \
  { tkn, #tkn }

namespace token {
enum class tkn_type {
  Err,  // this should never be placed in, indicates lex error
  Assignment,
  Iden,
  Num,
  String,
  Op_add,
  Op_sub,
  Op_mult,
  Op_div,
  Op_mod,
  Op_pow,
  Scope_start,
  Scope_end,
  End,
};

inline std::unordered_map<tkn_type, std::string> tkn_names = {
  TOKEN_STRING(tkn_type::Err),       TOKEN_STRING(tkn_type::Assignment),
  TOKEN_STRING(tkn_type::Iden),      TOKEN_STRING(tkn_type::Num),
  TOKEN_STRING(tkn_type::String),    TOKEN_STRING(tkn_type::Op_add),
  TOKEN_STRING(tkn_type::Op_sub),    TOKEN_STRING(tkn_type::Op_mult),
  TOKEN_STRING(tkn_type::Op_div),    TOKEN_STRING(tkn_type::Op_mod),
  TOKEN_STRING(tkn_type::Op_pow),    TOKEN_STRING(tkn_type::Scope_start),
  TOKEN_STRING(tkn_type::Scope_end), TOKEN_STRING(tkn_type::End),
};

class Token {
 public:
  tkn_type    type;
  std::string value;

  Token() { this->value = ""; };
  Token(std::string value) : value(value){};
  Token(tkn_type type, std::string value) : type(type), value(value){};

  friend inline auto operator==(const Token& lhs, const tkn_type& rhs) -> bool {
    return lhs.type == rhs;
  }

  inline auto toString() const -> std::string {
    return "[ " + tkn_names[this->type] + ": " + this->value +
           " ]";  // no elegant way to convert enum to string
  }
};
}  // namespace token

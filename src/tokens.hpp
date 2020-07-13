#pragma once

#define TOKEN_STRING(tkn) {tkn, #tkn}

namespace token {
  enum class tkn_type {
    ToRemove,
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
    End,
  };

  inline std::unordered_map<tkn_type, std::string> tkn_names = {
    TOKEN_STRING(tkn_type::ToRemove),
    TOKEN_STRING(tkn_type::Assignment),
    TOKEN_STRING(tkn_type::Iden),
    TOKEN_STRING(tkn_type::Num),
    TOKEN_STRING(tkn_type::String),
    TOKEN_STRING(tkn_type::Op_add),
    TOKEN_STRING(tkn_type::Op_sub),
    TOKEN_STRING(tkn_type::Op_mult),
    TOKEN_STRING(tkn_type::Op_div),
    TOKEN_STRING(tkn_type::Op_mod),
    TOKEN_STRING(tkn_type::Op_pow),
    TOKEN_STRING(tkn_type::End),
  };

  class Token {
  public:
    tkn_type type;
    std::string value;


    Token() {this->value="";};
    Token(std::string value)
      : value(value) {};
    Token(tkn_type type, std::string value)
      : type(type), value(value) {};

    inline auto toString() -> std::string {
      return "[ " + tkn_names[this->type] + ": " + this->value + " ]"; // no elegant way to convert enum to string
    }
  };
}

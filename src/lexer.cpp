#include "lexer.h"
#include "tokens.hpp"
#include "builtins.h"

auto lexer::lex(std::string args) -> std::vector<token::Token> {
  using namespace token;

  std::vector<Token> tokens;

  int pc = 0; // index of head
  int ipc = 0; // index of head initial pos
  while (pc < args.size()) {
    if (args[pc-1] == '<' && args[pc] == '-') {
      if (args[pc-2] != ' ')
        tokens.emplace_back(lexlet(args.substr(ipc, pc-ipc-1)));
      tokens.emplace_back(lexlet(args.substr(pc-1, 2)));
      ipc = pc+1;
    } else if (check(args[pc])) {
      tokens.emplace_back(lexlet(args.substr(ipc, pc-ipc)));
      if ((args[pc] != ' ' && pc-ipc != 1) || (args[pc-1] != ' '))
        tokens.emplace_back(lexlet(args.substr(pc, 1)));
      ipc = pc;
    }

    pc++;
  };
  tokens.emplace_back(lexlet(args.substr(ipc+1, args.size())));

  tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [](Token tkn) {
    return tkn.type == tkn_type::ToRemove;
  }), tokens.end());

  return tokens;
}

auto lexer::lexlet(std::string s) -> token::Token {
  using namespace token;
  Token token = Token(s);

  // to remove
  if (s == " " || s == "") {token.type = tkn_type::ToRemove; return token;}

  if (s == "<-") {token.type = tkn_type::Assignment; return token;}

  if (check_if_num(s)) {token.type = tkn_type::Num; return token;}

  //ops
  if (s == "+") {token.type = tkn_type::Op_add; return token;}
  if (s == "-") {token.type = tkn_type::Op_sub; return token;}
  if (s == "*") {token.type = tkn_type::Op_mult; return token;}
  if (s == "/") {token.type = tkn_type::Op_div; return token;}
  if (s == "%") {token.type = tkn_type::Op_mod; return token;}
  if (s == "^") {token.type = tkn_type::Op_pow; return token;}

  if (s == "\n" || s == "\r\n") {token.type = tkn_type::End; return token;}

  // default case
  token.type = tkn_type::Iden;
  return token;
}

auto lexer::check(char c) -> bool {
  return c == ' '
      || c == '+'
      || c == '-'
      || c == '*'
      || c == '/'
      || c == '^'
      || c == '%';
}

auto lexer::check_if_num(std::string s) -> bool {
  try {
    std::stoi(s);
    return true;
  } catch (...) {
    return false;
  }
}

// todo
auto lexer::check_if_string(std::string s) -> bool {
  return false;
}

#include <algorithm>
#include <cstddef>
#include <unordered_map>

#include "lexer.hpp"
#include "tokens.hpp"

auto check_if_math(std::string c) -> bool {
  return c == "+" || c == "-" || c == "*" || c == "/" || c == "^" || c == "%";
}

auto check_if_cond(std::string c) -> bool {
  return c == ">" || c == ">=" || c == "==" || c == "!=" || c == "<" || c == "<=";
}

auto check_if_string(std::string c) -> bool {
  // strings need to start with "
  return c[0] == '\"';
}

// todo: remove?
auto check_if_whitespace(std::string c) -> bool {
  return c == " " || c == "," || c == "\n" || c == "\r" || c == "\r\n";
}

auto check_if_num(std::string s) -> bool {
  try {
    std::stoi(s);
    return true;
  } catch (...) { return false; }
}

auto math_lexlet(std::string s) -> token::Token {
  using namespace token;
  Token token = Token(s);

  if (s == "+") {
    token.type = tkn_type::Op_add;
    return token;
  }
  if (s == "-") {
    token.type = tkn_type::Op_sub;
    return token;
  }
  if (s == "*") {
    token.type = tkn_type::Op_mult;
    return token;
  }
  if (s == "/") {
    token.type = tkn_type::Op_div;
    return token;
  }
  if (s == "%") {
    token.type = tkn_type::Op_mod;
    return token;
  }
  if (s == "^") {
    token.type = tkn_type::Op_pow;
    return token;
  }

  return token;
}

auto cond_lexlet(std::string s) -> token::Token {
  using namespace token;
  Token token = Token(s);

  if (s == ">") {
    token.type = tkn_type::Op_gt;
    return token;
  }
  if (s == ">=") {
    token.type = tkn_type::Op_gte;
    return token;
  }
  if (s == "==") {
    token.type = tkn_type::Op_eq;
    return token;
  }
  if (s == "!=") {
    token.type = tkn_type::Op_ne;
    return token;
  }
  if (s == "<") {
    token.type = tkn_type::Op_lt;
    return token;
  }
  if (s == "<=") {
    token.type = tkn_type::Op_lte;
    return token;
  }

  return token;
}

auto lexer::lex(std::string args) -> std::vector<token::Token> {
  using namespace token;

  std::vector<Token> tokens;

  if (args == "") return tokens;

  // split string to its components
  auto words       = args_splitter(args);
  bool string_flag = false;

  for (const auto& word : words) {

    // smelly
    if (string_flag) {
      tokens[tokens.size() - 1].value.append(" " + word);

      if (word[word.size() - 1] == '\"') string_flag = false;

      continue;
    }

    if (word == "\n" || word == "\r" || word == "\r\n") {
      tokens.emplace_back(Token(tkn_type::End, word));

      continue;
    }

    if (check_if_whitespace(word)) {
      // skip whitespace
      continue;
    }

    if (check_if_math(word)) {
      tokens.emplace_back(math_lexlet(word));
      continue;
    }

    if (check_if_string(word)) {
      tokens.emplace_back(Token(tkn_type::String, word));

      if (word[word.size() - 1] != '\"') string_flag = true;

      continue;
    }

    if (check_if_num(word)) {
      tokens.emplace_back(Token(tkn_type::Num, word));
      continue;
    }

    if (check_if_cond(word)) {
      tokens.emplace_back(cond_lexlet(word));
      continue;
    }

    if (word == "true" || word == "false") {
      tokens.emplace_back(Token(tkn_type::Bool, word));
      continue;
    }

    if (word == "fn") {
      tokens.emplace_back(Token(tkn_type::FuncDef, word));
      continue;
    }

    if (word == "for") {
      tokens.emplace_back(Token(tkn_type::For, word));
      continue;
    }

    if (word == "if") {
      tokens.emplace_back(Token(tkn_type::Cond, word));
      continue;
    }

    if (word == "<-") {
      tokens.emplace_back(Token(tkn_type::Assignment, word));
      continue;
    }

    if (word == "{") {
      tokens.emplace_back(Token(tkn_type::Scope_start, word));
      continue;
    }

    if (word == "}") {
      tokens.emplace_back(Token(tkn_type::Scope_end, word));
      continue;
    }

    if (word == "(") {
      tokens.emplace_back(Token(tkn_type::Param_list_start, word));
      continue;
    }

    if (word == ")") {
      tokens.emplace_back(Token(tkn_type::Param_list_end, word));
      continue;
    }

    // default
    tokens.emplace_back(Token(tkn_type::Iden, word));
  }

  if (string_flag) print_error("Still parsing string, was it terminated?");

  return tokens;
}

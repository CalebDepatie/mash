#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../common.hpp"

namespace token {
class Token;
}

namespace lexer {
auto lex(std::string args) -> std::vector<token::Token>;

// not sure if a faster / more efficient way to do this exists ?
inline std::unordered_map<std::string, int, imaphash, imapequal> identifiers;
}  // namespace lexer

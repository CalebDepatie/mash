#include "parser.h"
#include "tokens.hpp"

namespace parser {
  auto parse(std::vector<token::Token> tokens) -> parse_tree;
}

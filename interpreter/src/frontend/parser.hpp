#pragma once

#include "tokens.hpp"
#include "ast.hpp"
#include <memory>
#include <vector>

namespace parser {
auto parse(std::vector<token::Token>& tokens) -> std::shared_ptr<Node>;
}  // namespace parser

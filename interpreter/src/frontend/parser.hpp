#pragma once

#include "tokens.hpp"
#include "ast.hpp"
#include <memory>
#include <vector>

namespace parser {
auto parse(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Scope>;
}  // namespace parser

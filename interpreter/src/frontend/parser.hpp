#pragma once

#include <memory>
#include <vector>
#include "ast.hpp"
#include "tokens.hpp"

namespace parser {
auto parse(std::vector<token::Token>& tokens, int& pc) -> std::shared_ptr<Scope>;
}  // namespace parser

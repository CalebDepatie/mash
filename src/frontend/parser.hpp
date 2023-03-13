#pragma once

#include <memory>
#include <string>
#include <vector>

#include "tokens.hpp"

namespace parser {
class ASTNode {
 public:
  token::Token             token;
  std::shared_ptr<ASTNode> child;

  virtual auto toString(int depth = 0, bool newline = true) -> std::string;
  inline ASTNode();
  ASTNode(token::Token token);
  ASTNode(token::Token token, std::shared_ptr<ASTNode> child);
};
class ASTOp : public ASTNode {
 public:
  // treats child as left var
  std::shared_ptr<ASTNode> right;

  auto toString(int depth, bool newline) -> std::string;
  ASTOp(token::Token token, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right);
};

auto parse(std::vector<token::Token>& tokens) -> std::shared_ptr<ASTNode>;
}  // namespace parser

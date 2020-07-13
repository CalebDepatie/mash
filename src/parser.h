#pragma once

#include "tokens.hpp"

namespace parser {
  // using naked pointers for AST :flushed:
  class ASTNode {
  public:
    token::Token token;
    ASTNode*     child;

    virtual auto toString(int depth=0, bool newline=true) -> std::string;
    inline ASTNode() {};
    ASTNode(token::Token token);
    ASTNode(token::Token token, ASTNode* child);
    ~ASTNode();
  };
  class ASTOp : public ASTNode {
  public:
    // treats child as left var
    ASTNode* right;

    auto toString(int depth, bool newline) -> std::string;
    ASTOp(token::Token token, ASTNode* left, ASTNode* right);
    ~ASTOp();
  };

  auto parse(std::vector<token::Token>& tokens) -> ASTNode*;
}

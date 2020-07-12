#pragma once

namespace token {
  class Token;
}

namespace parser {
  class ASTNode {

  };

  class parse_tree {
    ASTNode* top;
  };

  auto parse(std::vector<token::Token> tokens) -> parse_tree;
}

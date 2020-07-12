#include "parser.h"

#define INDENT_SIZE 2

namespace parser {
  // generates a parse tree / AST from the tokens
  // recursive function ??
  auto parse(std::vector<token::Token> tokens) -> ASTNode* {
    ASTNode* ast = new ASTNode;
    ASTNode* bottom = ast;
    do {
      auto val = pop_front(tokens);
      // top
      if (ast->token.value == "") {
        ast->token = val;
        continue;
      }

      // setup operator case
      if (val.type == token::token_type::Value && tokens.size() >= 2) {
        if (tokens[0].type == token::token_type::Operator) {
          auto* left   = new ASTNode(val);
          auto op      = pop_front(tokens);
          val          = pop_front(tokens);
          auto* right  = new ASTNode(val);
          auto* parent = new ASTOp(op, left, right);

          bottom->child = parent;
          bottom = parent;
          continue;
        }
      }
      //default case
      auto* temp = new ASTNode(val);
      bottom->child = temp;
      bottom = temp;
    } while (tokens.size() != 0);
    return ast;
  }

  // constructors
  ASTNode::ASTNode(token::Token token)
    : token(token), child(nullptr) {}
  ASTNode::ASTNode(token::Token token, ASTNode* child)
    : token(token), child(child) {}
  ASTOp::ASTOp(token::Token token, ASTNode* left, ASTNode* right)
    : ASTNode(token, left), right(right) {}

  // destructors
  ASTNode::~ASTNode() {
    delete this->child;
  }
  ASTOp::~ASTOp() {
    delete this->right;
  }

  // to strings
  auto ASTNode::toString(int depth, bool newline) -> std::string {
    std::string indent = "";
    if (newline)
      indent += "\n";
    indent += std::string(depth*INDENT_SIZE, ' ');
    if (this->child != nullptr)
      return indent + this->token.toString() + this->child->toString(++depth, true);
    return indent + this->token.toString();
  }

  auto ASTOp::toString(int depth, bool newline) -> std::string {
    return "\n" + std::string((depth+=2)*INDENT_SIZE, ' ') + this->token.toString()
          + "\n" + this->child->toString(depth-2, false) + this->right->toString(depth+2, false);
  }
}

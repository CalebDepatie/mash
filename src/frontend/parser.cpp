#include <unordered_map>
#include "parser.hpp"

constexpr int INDENT_SIZE = 2;

namespace parser {
  // helper function checking if the token is an operation
  auto check_op(token::Token tkn) -> bool {
    using namespace token;
    return tkn.type == tkn_type::Op_add
        || tkn.type == tkn_type::Op_sub
        || tkn.type == tkn_type::Op_mult
        || tkn.type == tkn_type::Op_div
        || tkn.type == tkn_type::Op_mod
        || tkn.type == tkn_type::Op_pow;
  }

  // generates a parse tree / AST from the tokens
  // recursive function ??
  auto parse(std::vector<token::Token>& tokens) -> std::shared_ptr<ASTNode> {
    std::shared_ptr<ASTNode> ast(new ASTNode());
    std::shared_ptr<ASTNode> bottom = ast;
    std::vector<token::Token>::size_type pc = 0; // program / parse counter

    ast->token = tokens[pc++];

    do {
      // operator case
      if (tokens[pc].type == token::tkn_type::Num && tokens.size() >= 2) {
        if (check_op(tokens[pc+1])) {
          // im not entirely sure whats happening here yet
          std::shared_ptr<ASTNode> left(new ASTNode(tokens[pc++]));
          auto op = tokens[pc++];
          auto val = tokens[pc++];

          std::shared_ptr<ASTNode> right(new ASTNode(val));
          std::shared_ptr<ASTNode> parent(new ASTOp(op, left, right));

          bottom->child = parent;
          bottom        = parent;
          continue;
        }
      }

      // default case
      std::shared_ptr<ASTNode> temp(new ASTNode(tokens[pc++]));
      bottom->child = temp;
      bottom        = temp;

    } while (pc != tokens.size());

    return ast;
  }

  // constructors
  ASTNode::ASTNode()
    : child(nullptr) {}

  ASTNode::ASTNode(token::Token token)
    : token(token), child(nullptr) {}

  ASTNode::ASTNode(token::Token token, std::shared_ptr<ASTNode> child)
    : token(token), child(child) {}

  ASTOp::ASTOp(token::Token token, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right)
    : ASTNode(token, left), right(right) {}

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
    depth+=2;
    return "\n" + std::string(depth*INDENT_SIZE, ' ') + this->token.toString()
          + "\n" + this->child->toString(depth-2, false) + this->right->toString(depth+2, false);
  }
}

#include "middleend.hpp"

#include <filesystem>

auto Execution_Key::toString() -> std::string {
  return op_names[this->op] + ": " + this->value;
}

auto bakeAST(std::shared_ptr<parser::ASTNode> execTop) -> std::vector<Execution_Key> {
  auto exec_vec = std::vector<Execution_Key>();

  // set up
  exec_vec.emplace_back(Execution_Key{
    op: Operation::Begin,
    value: std::filesystem::current_path(),
  });

  // fill up execution list from the tree
  std::shared_ptr<parser::ASTNode> cur_node = execTop;
  for (;;) {
    if (!cur_node)
      break;

    cur_node = cur_node->child;
  }

  // instruct to end task
  exec_vec.emplace_back(Execution_Key{
    op: Operation::End,
  });

  return exec_vec;
}

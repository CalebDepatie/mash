#include "middleend.hpp"
#include "frontend/tokens.hpp"

#include <filesystem>

auto bakeScope(std::shared_ptr<parser::Scope> node) -> std::vector<Execution_Key>;
auto bakeAsmt(std::shared_ptr<parser::Asmt> node) -> std::vector<Execution_Key>;
auto bakeMath(std::shared_ptr<parser::Math> node) -> std::vector<Execution_Key>;
auto bakeIfCond(std::shared_ptr<parser::IfCond> node) -> std::vector<Execution_Key>;
auto bakeLoop(std::shared_ptr<parser::Loop> node) -> std::vector<Execution_Key>;
auto bakeFnCall(std::shared_ptr<parser::FnCall> node) -> std::vector<Execution_Key>;
auto bakeValue(std::shared_ptr<parser::Value> node) -> std::vector<Execution_Key>;
auto bakeFnDef(std::shared_ptr<parser::FnDef> node) -> std::vector<Execution_Key>;

auto Execution_Key::toString() -> std::string {
  return op_names[this->op] + ": " + this->value;
}

auto concat(const std::vector<Execution_Key>& a, const std::vector<Execution_Key>& b)
  -> std::vector<Execution_Key> {
  std::vector<Execution_Key> res;

  res.reserve(a.size() + b.size()); // reserve for some allocation efficiency

  res.insert(res.end(), a.begin(), a.end());
  res.insert(res.end(), b.begin(), b.end());

  return res;
}

auto bakeAST(std::shared_ptr<parser::Scope> execTop) -> std::vector<Execution_Key> {
  auto exec_vec = std::vector<Execution_Key>();

  // set up
  exec_vec.emplace_back(Execution_Key{
    op: Operation::Begin,
    value: std::filesystem::current_path(),
  });

  // fill up execution list from the tree
  exec_vec = concat(exec_vec, bakeScope(execTop));

  // instruct to end task
  exec_vec.emplace_back(Execution_Key{
    op: Operation::End,
  });

  return exec_vec;
}

// wrapper around scope for nested operation, like parsing
auto bakeScope(std::shared_ptr<parser::Scope> node) -> std::vector<Execution_Key> {
  auto cur_ops = std::vector<Execution_Key>();

  cur_ops.emplace_back(Execution_Key{
    op: Operation::ScopeStart,
  });

  std::shared_ptr<parser::Node> cur_node = node->line_top;
  for (;;) {
    if (!cur_node) // exit the loop if theres no more nodes
      break;

    if (auto asmt = std::dynamic_pointer_cast<parser::Asmt>(cur_node)) {
      cur_ops = concat(cur_ops, bakeAsmt(asmt));

    } else if (auto fncall = std::dynamic_pointer_cast<parser::FnCall>(cur_node)) {
      cur_ops = concat(cur_ops, bakeFnCall(fncall));

    }

    cur_ops.emplace_back(Execution_Key{
      op: Operation::ClearReg,
    });

    cur_node = cur_node->next;
  }

  cur_ops.emplace_back(Execution_Key{
    op: Operation::ScopeEnd,
  });

  return cur_ops;
}

auto bakeAsmt(std::shared_ptr<parser::Asmt> node) -> std::vector<Execution_Key> {
  auto cur_ops = std::vector<Execution_Key>();

  cur_ops.emplace_back(Execution_Key{
    op: Operation::Asmt,
    value: node->iden,
  });

  if (auto value = std::get_if<std::shared_ptr<parser::Value>>(&node->val)) {
    cur_ops = concat(cur_ops, bakeValue(*value));

  } else if (auto fndef = std::get_if<std::shared_ptr<parser::FnDef>>(&node->val)) {
    cur_ops = concat(cur_ops, bakeFnDef(*fndef));
  }

  return cur_ops;
}

auto bakeMath(std::shared_ptr<parser::Math> node) -> std::vector<Execution_Key> {
  auto cur_ops = std::vector<Execution_Key>();

  return cur_ops;
}

auto bakeIfCond(std::shared_ptr<parser::IfCond> node) -> std::vector<Execution_Key> {
  auto cur_ops = std::vector<Execution_Key>();

  return cur_ops;
}

auto bakeLoop(std::shared_ptr<parser::Loop> node) -> std::vector<Execution_Key> {
  auto cur_ops = std::vector<Execution_Key>();

  return cur_ops;
}

auto bakeFnCall(std::shared_ptr<parser::FnCall> node) -> std::vector<Execution_Key> {
  auto cur_ops = std::vector<Execution_Key>();

  cur_ops.emplace_back(Execution_Key{
    op: Operation::Recall,
    value: node->iden,
  });

  for (const auto& arg : node->args) {
    if (auto value = std::get_if<std::shared_ptr<parser::Value>>(&arg)) {
      cur_ops = concat(cur_ops, bakeValue(*value));

    } else if (auto scope = std::get_if<std::shared_ptr<parser::Scope>>(&arg)) {
      cur_ops = concat(cur_ops, bakeScope(*scope));

    }
  }

  return cur_ops;
}

auto bakeValue(std::shared_ptr<parser::Value> node) -> std::vector<Execution_Key> {
  auto cur_ops = std::vector<Execution_Key>();

  if (auto num = std::dynamic_pointer_cast<parser::Number>(node)) {
    cur_ops.emplace_back(Execution_Key{
      op: Operation::NumberVal,
      value: std::to_string(num->val),
    });


  } else if (auto string = std::dynamic_pointer_cast<parser::String>(node)) {
    cur_ops.emplace_back(Execution_Key{
      op: Operation::StringVal,
      value: string->val,
    });

  } else if (auto iden = std::dynamic_pointer_cast<parser::NamedVal>(node)) {
    cur_ops.emplace_back(Execution_Key{
      op: Operation::Recall,
      value: iden->iden,
    });

  } else if (auto boolean = std::dynamic_pointer_cast<parser::Boolean>(node)) {
    cur_ops.emplace_back(Execution_Key{
      op: Operation::BoolVal,
      value: boolean->val ? "true" : "false",
    });

  } else if (auto math = std::dynamic_pointer_cast<parser::Math>(node)) {


  } else if (auto cond = std::dynamic_pointer_cast<parser::Conditional>(node)) {


  } else if (auto range = std::dynamic_pointer_cast<parser::Range>(node)) {


  } else if (auto fncall = std::dynamic_pointer_cast<parser::FnCall>(node)) {

  }

  return cur_ops;
}

auto bakeFnDef(std::shared_ptr<parser::FnDef> node) -> std::vector<Execution_Key> {
  auto cur_ops = std::vector<Execution_Key>();

  return cur_ops;
}

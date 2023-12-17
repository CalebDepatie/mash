#include "middleend.hpp"
#include "frontend/parser.hpp"
#include "frontend/tokens.hpp"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

auto bakeScope(std::shared_ptr<parser::Scope> node) -> std::vector<ExecKey>;
auto bakeAsmt(std::shared_ptr<parser::Asmt> node) -> std::vector<ExecKey>;
auto bakeMath(std::shared_ptr<parser::Math> node) -> std::vector<ExecKey>;
auto bakeCond(std::shared_ptr<parser::Conditional> node) -> std::vector<ExecKey>;
auto bakeIfCond(std::shared_ptr<parser::IfCond> node) -> std::vector<ExecKey>;
auto bakeLoop(std::shared_ptr<parser::Loop> node) -> std::vector<ExecKey>;
auto bakeFnCall(std::shared_ptr<parser::FnCall> node) -> std::vector<ExecKey>;
auto bakeValue(std::shared_ptr<parser::Value> node) -> std::vector<ExecKey>;
auto bakeFnDef(std::shared_ptr<parser::FnDef> node) -> std::vector<ExecKey>;

auto concat(const std::vector<ExecKey>& a, const std::vector<ExecKey>& b) -> std::vector<ExecKey> {
  std::vector<ExecKey> res;

  res.reserve(a.size() + b.size());  // reserve for some allocation efficiency

  res.insert(res.end(), a.begin(), a.end());
  res.insert(res.end(), b.begin(), b.end());

  return res;
}

auto bakeAST(std::shared_ptr<parser::Scope> execTop) -> ExecStream {
  GOOGLE_PROTOBUF_VERIFY_VERSION;  // verify compat

  auto exec_vec = std::vector<ExecKey>();

  // set up
  ExecKey begin_key;
  begin_key.set_op(Operation::Begin);
  exec_vec.emplace_back(begin_key);

  // fill up execution list from the tree
  exec_vec = concat(exec_vec, bakeScope(execTop));

  // instruct to end task
  ExecKey end_key;
  end_key.set_op(Operation::End);
  exec_vec.emplace_back(end_key);

  ExecStream ret_stream;
  ret_stream.set_currentworkingdir(std::filesystem::current_path());

  // FIXME: this is simply inefficient
  for (const auto& key : exec_vec) {
    auto exec_key = ret_stream.add_execkeys();
    exec_key->CopyFrom(key);
  }

  return ret_stream;
}

// wrapper around scope for nested operation, like parsing
auto bakeScope(std::shared_ptr<parser::Scope> node) -> std::vector<ExecKey> {
  auto cur_ops = std::vector<ExecKey>();

  ExecKey scope_start;
  scope_start.set_op(Operation::ScopeStart);
  cur_ops.emplace_back(scope_start);

  ExecKey clear_reg;
  clear_reg.set_op(Operation::ClearReg);
  cur_ops.emplace_back(clear_reg);

  std::shared_ptr<parser::Node> cur_node = node->line_top;
  for (;;) {
    if (!cur_node)  // exit the loop if theres no more nodes
      break;

    if (auto asmt = std::dynamic_pointer_cast<parser::Asmt>(cur_node)) {
      cur_ops = concat(cur_ops, bakeAsmt(asmt));

    } else if (auto fncall = std::dynamic_pointer_cast<parser::FnCall>(cur_node)) {
      cur_ops = concat(cur_ops, bakeFnCall(fncall));

    } else if (auto ifcond = std::dynamic_pointer_cast<parser::IfCond>(cur_node)) {
      cur_ops = concat(cur_ops, bakeIfCond(ifcond));

    } else if (auto loop = std::dynamic_pointer_cast<parser::Loop>(cur_node)) {
      cur_ops = concat(cur_ops, bakeLoop(loop));
    }

    cur_ops.emplace_back(clear_reg);

    cur_node = cur_node->next;
  }

  ExecKey scope_end;
  scope_end.set_op(Operation::ScopeEnd);
  cur_ops.emplace_back(scope_end);
  cur_ops.emplace_back(clear_reg);

  return cur_ops;
}

auto bakeAsmt(std::shared_ptr<parser::Asmt> node) -> std::vector<ExecKey> {
  auto cur_ops = std::vector<ExecKey>();

  ExecKey asmt;
  asmt.set_op(Operation::Asmt);
  asmt.set_stringvalue(node->iden);
  cur_ops.emplace_back(asmt);

  if (auto value = std::get_if<std::shared_ptr<parser::Value>>(&node->val)) {
    cur_ops = concat(cur_ops, bakeValue(*value));

  } else if (auto fndef = std::get_if<std::shared_ptr<parser::FnDef>>(&node->val)) {
    cur_ops = concat(cur_ops, bakeFnDef(*fndef));
  }

  return cur_ops;
}

auto bakeMath(std::shared_ptr<parser::Math> node) -> std::vector<ExecKey> {
  auto cur_ops = std::vector<ExecKey>();

  std::string math_op;

  switch (node->operation) {
    case token::tkn_type::Op_add: {
      math_op = "+";
      break;
    }
    case token::tkn_type::Op_sub: {
      math_op = "-";
      break;
    }
    case token::tkn_type::Op_div: {
      math_op = "/";
      break;
    }
    case token::tkn_type::Op_mult: {
      math_op = "*";
      break;
    }
    case token::tkn_type::Op_mod: {
      math_op = "%";
      break;
    }
    case token::tkn_type::Op_pow: {
      math_op = "^";
      break;
    }
    default: {
    }
  }

  ExecKey math;
  math.set_op(Operation::Math);
  math.set_stringvalue(math_op);
  cur_ops.emplace_back(math);

  // baking value instead of a number/math to leave it a bit more open in the future
  auto coerceToValue =
    [](std::variant<std::shared_ptr<parser::Number>, std::shared_ptr<parser::NamedVal>,
                    std::shared_ptr<parser::Math>>
         n) -> std::shared_ptr<parser::Value> {
    if (auto num = std::get_if<std::shared_ptr<parser::Number>>(&n)) {
      return *num;

    } else if (auto iden = std::get_if<std::shared_ptr<parser::NamedVal>>(&n)) {
      return *iden;

    } else if (auto math = std::get_if<std::shared_ptr<parser::Math>>(&n)) {
      return *math;
    }
  };

  cur_ops = concat(cur_ops, bakeValue(coerceToValue(node->left)));
  cur_ops = concat(cur_ops, bakeValue(coerceToValue(node->right)));

  return cur_ops;
}

auto bakeIfCond(std::shared_ptr<parser::IfCond> node) -> std::vector<ExecKey> {
  auto cur_ops = std::vector<ExecKey>();

  ExecKey if_key;
  if_key.set_op(Operation::If);
  cur_ops.emplace_back(if_key);

  cur_ops = concat(cur_ops, bakeCond(node->cond));

  // for consistency. May be unessecary
  ExecKey clear_reg;
  clear_reg.set_op(Operation::ClearReg);
  cur_ops.emplace_back(clear_reg);

  cur_ops = concat(cur_ops, bakeScope(node->scope));

  return cur_ops;
}

auto bakeLoop(std::shared_ptr<parser::Loop> node) -> std::vector<ExecKey> {
  auto cur_ops = std::vector<ExecKey>();

  ExecKey loop;
  loop.set_op(Operation::Loop);
  cur_ops.emplace_back(loop);

  cur_ops = concat(cur_ops, bakeAsmt(node->asmt));

  // for consistency. May be unessecary
  ExecKey clear_reg;
  clear_reg.set_op(Operation::ClearReg);
  cur_ops.emplace_back(clear_reg);

  cur_ops = concat(cur_ops, bakeScope(node->scope));

  return cur_ops;
}

auto bakeFnCall(std::shared_ptr<parser::FnCall> node) -> std::vector<ExecKey> {
  auto cur_ops = std::vector<ExecKey>();

  ExecKey recall;
  recall.set_op(Operation::FnCall);
  recall.set_stringvalue(node->iden);
  cur_ops.emplace_back(recall);

  for (const auto& arg : node->args) {
    if (auto value = std::get_if<std::shared_ptr<parser::Value>>(&arg)) {
      cur_ops = concat(cur_ops, bakeValue(*value));

    } else if (auto scope = std::get_if<std::shared_ptr<parser::Scope>>(&arg)) {
      cur_ops = concat(cur_ops, bakeScope(*scope));
    }
  }

  return cur_ops;
}

auto bakeValue(std::shared_ptr<parser::Value> node) -> std::vector<ExecKey> {
  auto cur_ops = std::vector<ExecKey>();

  if (auto num = std::dynamic_pointer_cast<parser::Number>(node)) {
    ExecKey num_key;
    num_key.set_op(Operation::NumberVal);
    num_key.set_numbervalue(num->val);
    cur_ops.emplace_back(num_key);

  } else if (auto string = std::dynamic_pointer_cast<parser::String>(node)) {
    ExecKey string_key;
    string_key.set_op(Operation::StringVal);
    string_key.set_stringvalue(string->val);
    cur_ops.emplace_back(string_key);

  } else if (auto iden = std::dynamic_pointer_cast<parser::NamedVal>(node)) {
    ExecKey recall;
    recall.set_op(Operation::Recall);
    recall.set_stringvalue(iden->iden);
    cur_ops.emplace_back(recall);

  } else if (auto boolean = std::dynamic_pointer_cast<parser::Boolean>(node)) {
    ExecKey bool_key;
    bool_key.set_op(Operation::BoolVal);
    bool_key.set_booleanvalue(boolean->val);
    cur_ops.emplace_back(bool_key);

  } else if (auto math = std::dynamic_pointer_cast<parser::Math>(node)) {
    cur_ops = concat(cur_ops, bakeMath(math));

  } else if (auto cond = std::dynamic_pointer_cast<parser::Conditional>(node)) {
    cur_ops = concat(cur_ops, bakeCond(cond));

  } else if (auto range = std::dynamic_pointer_cast<parser::Range>(node)) {
    ExecKey range_key;
    range_key.set_op(Operation::RangeVal);

    Range r;
    r.set_from(range->from->val);
    r.set_to(range->to->val);
    range_key.set_allocated_rangevalue(&r);

    cur_ops.emplace_back(range_key);

  } else if (auto fncall = std::dynamic_pointer_cast<parser::FnCall>(node)) {
    cur_ops = concat(cur_ops, bakeFnCall(fncall));
  }

  return cur_ops;
}

auto bakeFnDef(std::shared_ptr<parser::FnDef> node) -> std::vector<ExecKey> {
  auto cur_ops = std::vector<ExecKey>();

  std::string args = "";

  for (int i = 0; i < node->arg_names.size(); i++) {
    args += node->arg_names[i];

    if (i < node->arg_names.size() - 1)
      args += ",";
  }

  ExecKey def_key;
  def_key.set_op(Operation::FnDef);
  def_key.set_stringvalue(args);
  cur_ops.emplace_back(def_key);

  cur_ops = concat(cur_ops, bakeScope(node->scope));

  return cur_ops;
}

auto bakeCond(std::shared_ptr<parser::Conditional> node) -> std::vector<ExecKey> {
  auto cur_ops = std::vector<ExecKey>();

  std::string cond_op;

  switch (node->operation) {
    case token::tkn_type::Op_eq: {
      cond_op = "==";
      break;
    }
    case token::tkn_type::Op_ne: {
      cond_op = "!=";
      break;
    }
    case token::tkn_type::Op_gt: {
      cond_op = ">";
      break;
    }
    case token::tkn_type::Op_gte: {
      cond_op = ">=";
      break;
    }
    case token::tkn_type::Op_lt: {
      cond_op = "<";
      break;
    }
    case token::tkn_type::Op_lte: {
      cond_op = "<=";
      break;
    }

    default: {
    }
  }

  ExecKey cond_key;
  cond_key.set_op(Operation::Cond);
  cond_key.set_stringvalue(cond_op);
  cur_ops.emplace_back(cond_key);

  cur_ops = concat(cur_ops, bakeValue(node->left));
  cur_ops = concat(cur_ops, bakeValue(node->right));

  return cur_ops;
}

#pragma once

#include <memory>

#include "execStream.pb.h"
#include "frontend/ast.hpp"

auto bakeAST(std::shared_ptr<parser::Scope> execTop) -> ExecStream;

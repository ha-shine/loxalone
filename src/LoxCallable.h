//
// Created by Htet Aung Shine on 16/12/2022.
//

#ifndef LOXALONE_LOXCALLABLE_H
#define LOXALONE_LOXCALLABLE_H

#include <utility>
#include <vector>

#include "Environment.h"
#include "Stmt.h"
#include "Token.h"

namespace loxalone {

class Interpreter;

class LoxCallable {
 public:
  virtual auto arity() -> int = 0;
  virtual auto execute(Interpreter&, const std::vector<lox_literal>&)
      -> lox_literal = 0;
  virtual auto name() -> std::string_view = 0;
};

// LoxFunction implements a function object for loxalone
class LoxFunction : public LoxCallable {
 private:
  FunctionPtr declaration;
  Environment closure;

 public:
  explicit LoxFunction(FunctionPtr&& declaration, Environment closure)
      : declaration{std::move(declaration)}, closure{std::move(closure)} {}

  auto arity() -> int override;
  auto execute(Interpreter&, const std::vector<lox_literal>& args)
      -> lox_literal override;
  auto name() -> std::string_view override;
};

// NativeCallable is a helper class that bridges the C++ callables with the
// lox interpreter
class NativeCallable : public LoxCallable {
 private:
  using func = std::function<lox_literal(const std::vector<lox_literal>&)>;

  const std::string name_m;
  int arity_m;
  func fun_m;

 public:
  NativeCallable(std::string_view name, int arity, func&& fun)
      : name_m{name}, arity_m{arity}, fun_m{std::move(fun)} {}

  auto arity() -> int override { return arity_m; }

  auto execute(Interpreter&, const std::vector<lox_literal>& args)
      -> lox_literal override {
    return fun_m(args);
  }

  auto name() -> std::string_view override { return name_m; }
};

}  // namespace loxalone

#endif  // LOXALONE_LOXCALLABLE_H

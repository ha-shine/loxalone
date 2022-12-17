//
// Created by Htet Aung Shine on 16/12/2022.
//

#ifndef LOXALONE_LOXCALLABLE_H
#define LOXALONE_LOXCALLABLE_H

#include <vector>

#include "Stmt.h"
#include "Token.h"

class Interpreter;

class LoxCallable {
 public:
  virtual auto arity() -> int = 0;
  virtual auto execute(Interpreter&, const std::vector<lox_literal>&) -> lox_literal = 0;
};

// LoxFunction implements a function object for loxalone
class LoxFunction : public LoxCallable {
 private:
  FunctionPtr declaration;

 public:
  explicit LoxFunction(FunctionPtr&& declaration) : declaration{std::move(declaration)} {}

  auto arity() -> int override;
  auto execute(Interpreter&, const std::vector<lox_literal>& args)
      -> lox_literal override;
};

// NativeCallable is a helper class that bridges the C++ callables with the
// lox interpreter
class NativeCallable : public LoxCallable {
 private:
  using func = std::function<lox_literal(const std::vector<lox_literal>&)>;

  int arity_m;
  func fun_m;

 public:
  NativeCallable(int arity, func&& fun)
      : arity_m{arity}, fun_m{std::move(fun)} {}

  auto arity() -> int override { return arity_m; }

  auto execute(Interpreter&, const std::vector<lox_literal>& args) -> lox_literal override {
    return fun_m(args);
  }
};

#endif  // LOXALONE_LOXCALLABLE_H

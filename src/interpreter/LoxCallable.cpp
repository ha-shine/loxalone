//
// Created by Htet Aung Shine on 16/12/2022.
//

#include "LoxCallable.h"

#include "Interpreter.h"

namespace loxalone {

auto LoxFunction::arity() const -> int {
  return static_cast<int>(declaration->params_m.size());
}

auto LoxFunction::execute(Interpreter& interpreter,
                          const std::vector<lox_literal>& args) -> lox_literal {
  Environment env{&closure};
  for (int i = 0; i < declaration->params_m.size(); i++) {
    env.define(declaration->params_m[i].lexeme, args[i]);
  }

  try {
    interpreter.execute_block(declaration->body_m, &env);
  } catch (ReturnObject& ret) {
    return ret.value;
  }

  return std::monostate{};
}

auto LoxFunction::name() const -> std::string_view {
  return declaration->name_m.lexeme;
}

}  // namespace loxalone

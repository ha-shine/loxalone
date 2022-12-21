//
// Created by Htet Aung Shine on 21/12/2022.
//

#include "LoxClass.h"

#include "LoxInstance.h"

namespace loxalone {

auto LoxClass::name() const -> std::string_view { return name_m; }

// TODO: Arity of this?
int LoxClass::arity() const { return 0; }

lox_literal LoxClass::execute(Interpreter& interpreter,
                              const std::vector<lox_literal>& vector) {
  return std::make_unique<LoxInstance>(*this);
}

}  // namespace loxalone

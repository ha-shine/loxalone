//
// Created by Htet Aung Shine on 21/12/2022.
//

#ifndef LOXALONE_LOXCLASS_H
#define LOXALONE_LOXCLASS_H

#include <string>
#include <utility>

#include "LoxCallable.h"

namespace loxalone {

class LoxClass: public LoxCallable {
 private:
  const std::string name_m;

 public:
  explicit LoxClass(std::string name): name_m{std::move(name)} {}
  auto name() const -> std::string_view override;

  auto arity() const -> int override;
  auto execute(Interpreter& interpreter, const std::vector<lox_literal>& vector)
      -> lox_literal override;
};

}

#endif  // LOXALONE_LOXCLASS_H

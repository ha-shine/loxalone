//
// Created by Htet Aung Shine on 16/12/2022.
//

#ifndef LOXALONE_LOXCALLABLE_H
#define LOXALONE_LOXCALLABLE_H

#include <vector>

#include "Token.h"

class LoxCallable {
 public:
  virtual auto arity() -> int = 0;
  virtual auto call_func(const std::vector<lox_literal>&) -> lox_literal = 0;
};

#endif  //LOXALONE_LOXCALLABLE_H

//
// Created by Htet Aung Shine on 21/12/2022.
//

#ifndef LOXALONE_LOXINSTANCE_H
#define LOXALONE_LOXINSTANCE_H

#include "LoxClass.h"

namespace loxalone {

class LoxInstance {
 public:
  explicit LoxInstance(const LoxClass& cls) : cls{cls} {}

  const LoxClass& cls;
};

}  // namespace loxalone

#endif  // LOXALONE_LOXINSTANCE_H

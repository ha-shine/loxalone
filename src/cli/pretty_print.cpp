//
// Created by Htet Aung Shine on 30/11/22.
//

#include <iostream>

#include "../interpreter/PrettyPrinter.h"

using namespace loxalone;

int main() {
  Expr expr = Binary::create(Literal::create(2.0),
                             Token{TokenType::PLUS, "+", std::nullopt, 1},
                             Literal::create(2.0));

  PrettyPrinter printer{};
  std::cout << visit<std::string>(printer, expr) << "\n";
  return 0;
}
//
// Created by Htet Aung Shine on 30/11/22.
//

#include "../PrettyPrinter.h"

#include <iostream>

int main() {
  Expr expr{std::make_unique<BinaryExpr>(
      std::make_unique<LiteralVal>(LiteralVal{2.0}),
      Token{TokenType::PLUS, "+", std::nullopt, 1},
      std::make_unique<LiteralVal>(LiteralVal{2.0}))};

  PrettyPrinter printer{};
  std::cout << visit<std::string>(printer, expr) << "\n";
  return 0;
}
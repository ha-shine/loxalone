//
// Created by Htet Aung Shine on 23/11/2022.
//

#ifndef LOXALONE_ERROR_H
#define LOXALONE_ERROR_H

#include <fmt/format.h>

#include <exception>
#include <string>

#include "Token.h"

namespace loxalone {

class RuntimeError : std::exception {
 public:
  RuntimeError(Token token, std::string_view msg)
      : token{std::move(token)}, msg{msg} {}

  std::string msg;
  const Token token;
};

static auto report(int line, std::string_view where,
                   const std::string_view& msg) {
  fmt::print(stderr, "[line {}] Error{}: {}\n", line, where, msg);
}

static auto error(int line, std::string_view msg) { report(line, "", msg); }

}

#endif  // LOXALONE_ERROR_H

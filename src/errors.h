//
// Created by Htet Aung Shine on 23/11/2022.
//

#ifndef LOXALONE_ERRORS_H
#define LOXALONE_ERRORS_H

#include <exception>
#include <string>

auto report(int line, const std::string_view& where, const std::string_view& msg) {
  fmt::print(stderr, "[line {}] Error{}: {}\n", line, where, msg);
}

auto error(int line, const std::string_view& msg) {
  report(line, "", msg);
}

#endif  //LOXALONE_ERRORS_H

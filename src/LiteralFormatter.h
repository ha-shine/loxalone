//
// Created by Htet Aung Shine on 18/12/2022.
//

#ifndef LOXALONE_LITERALFORMATTER_H
#define LOXALONE_LITERALFORMATTER_H

#include "Token.h"
#include "LoxCallable.h"

#include <fmt/format.h>

// This file implements LiteralFormatter and other related methods for
// fmt::format to work with lox_literal values. This is required for
// `print` statements in the interpreter.

// Formatter implementation for std::monostate
template <>
struct fmt::formatter<std::monostate> {
  constexpr auto parse(fmt::format_parse_context& ctx)
      -> decltype(ctx.begin()) {
    return ctx.end();
  }

  template <typename FormatContext>
  auto format(const std::monostate& token, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "std::monostate");
  }
};

// Formatter implementation for std::shared_ptr<LoxCallable>
template <>
struct fmt::formatter<CallablePtr> {
  constexpr auto parse(fmt::format_parse_context& ctx)
      -> decltype(ctx.begin()) {
    return ctx.end();
  }

  template <typename FormatContext>
  auto format(const std::shared_ptr<LoxCallable>& token, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "<fun {}>", token->name());
  }
};

template<typename FormatContext>
class LiteralFormatter {
 private:
  FormatContext& ctx;

 public:
  auto operator()(const auto& arg) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}\n", arg);
  }

  LiteralFormatter(FormatContext& ctx): ctx{ctx} {}
};

// Formatter implementation for lox_literal
template <>
struct fmt::formatter<lox_literal> {
  constexpr auto parse(fmt::format_parse_context& ctx)
      -> decltype(ctx.begin()) {
    return ctx.end();
  }

  template <typename FormatContext>
  auto format(const lox_literal& val, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return std::visit(LiteralFormatter(ctx), val);
  }
};

#endif  // LOXALONE_LITERALFORMATTER_H

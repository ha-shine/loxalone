//
// Created by Htet Aung Shine on 18/12/2022.
//

#ifndef LOXALONE_LITERALFORMATTER_H
#define LOXALONE_LITERALFORMATTER_H

#include <fmt/format.h>

#include "LoxCallable.h"
#include "LoxClass.h"
#include "LoxInstance.h"
#include "Token.h"

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
struct fmt::formatter<loxalone::LoxCallablePtr> {
  constexpr auto parse(fmt::format_parse_context& ctx)
      -> decltype(ctx.begin()) {
    return ctx.end();
  }

  template <typename FormatContext>
  auto format(const loxalone::LoxCallablePtr& token, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    if (auto* ptr = dynamic_cast<loxalone::LoxClass*>(token.get());
        ptr != nullptr) {
      return fmt::format_to(ctx.out(), "<class {}>", ptr->name());
    }

    return fmt::format_to(ctx.out(), "<fun {}>", token->name());
  }
};

// Formatter implementation for std::shared_ptr<LoxInstance>
template <>
struct fmt::formatter<loxalone::LoxInstancePtr> {
  constexpr auto parse(fmt::format_parse_context& ctx)
      -> decltype(ctx.begin()) {
    return ctx.end();
  }

  template <typename FormatContext>
  auto format(const loxalone::LoxInstancePtr& token, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "<instance {}.class>", token.get()->cls.name());
  }
};

template <typename FormatContext>
class LiteralFormatter {
 private:
  FormatContext& ctx;

 public:
  auto operator()(const auto& arg) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}\n", arg);
  }

  LiteralFormatter(FormatContext& ctx) : ctx{ctx} {}
};

// Formatter implementation for lox_literal
template <>
struct fmt::formatter<loxalone::lox_literal> {
  constexpr auto parse(fmt::format_parse_context& ctx)
      -> decltype(ctx.begin()) {
    return ctx.end();
  }

  template <typename FormatContext>
  auto format(const loxalone::lox_literal& val, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return std::visit(LiteralFormatter(ctx), val);
  }
};

#endif  // LOXALONE_LITERALFORMATTER_H

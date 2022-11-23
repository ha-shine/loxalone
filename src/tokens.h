//
// Created by Htet Aung Shine on 23/11/2022.
//

#ifndef LOXALONE_TOKENS_H
#define LOXALONE_TOKENS_H

#include <fmt/format.h>
#include <optional>
#include <string>
#include <variant>


using lox_literal = std::variant<std::string, double>;

enum TokenType {
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  IDENTIFIER,
  STRING,
  NUMBER,

  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  EOF_CHAR
};

constexpr const char* tt_to_string(TokenType type) {
  switch (type) {
    case TokenType::LEFT_PAREN:
      return "LEFT_PAREN";
    case TokenType::RIGHT_PAREN:
      return "RIGHT_PAREN";
    case TokenType::LEFT_BRACE:
      return "LEFT_BRACE";
    case TokenType::RIGHT_BRACE:
      return "RIGHT_BRACE";
    case TokenType::COMMA:
      return "COMMA";
    case TokenType::DOT:
      return "DOT";
    case TokenType::MINUS:
      return "MINUS";
    case TokenType::PLUS:
      return "PLUS";
    case TokenType::SEMICOLON:
      return "SEMICOLON";
    case TokenType::SLASH:
      return "SLASH";
    case TokenType::STAR:
      return "STAR";

    case TokenType::BANG:
      return "BANG";
    case TokenType::BANG_EQUAL:
      return "BANG_EQUAL";
    case TokenType::EQUAL:
      return "EQUAL";
    case TokenType::EQUAL_EQUAL:
      return "EQUAL_EQUAL";
    case TokenType::GREATER:
      return "GREATER";
    case TokenType::GREATER_EQUAL:
      return "GREATER_EQUAL";
    case TokenType::LESS:
      return "LESS";
    case TokenType::LESS_EQUAL:
      return "LESS_EQUAL";

    case TokenType::IDENTIFIER:
      return "IDENTIFIER";
    case TokenType::STRING:
      return "STRING";
    case TokenType::NUMBER:
      return "NUMBER";

    case TokenType::AND:
      return "AND";
    case TokenType::CLASS:
      return "CLASS";
    case TokenType::ELSE:
      return "ELSE";
    case TokenType::FALSE:
      return "FALSE";
    case TokenType::FUN:
      return "FUN";
    case TokenType::FOR:
      return "FOR";
    case TokenType::IF:
      return "IF";
    case TokenType::NIL:
      return "NIL";
    case TokenType::OR:
      return "OR";
    case TokenType::PRINT:
      return "PRINT";
    case TokenType::RETURN:
      return "RETURN";
    case TokenType::SUPER:
      return "SUPER";
    case TokenType::THIS:
      return "THIS";
    case TokenType::TRUE:
      return "TRUE";
    case TokenType::VAR:
      return "VAR";
    case TokenType::WHILE:
      return "WHILE";

    case TokenType::EOF_CHAR:
      return "EOF_CHAR";
  }
}

struct Token {
  const TokenType type;
  const std::string lexeme;
  const std::optional<lox_literal> literal;
  const int line;

  Token(TokenType type, std::string&& lexeme,
        std::optional<lox_literal>&& literal, int line)
      : type{type}, lexeme{lexeme}, literal{literal}, line{line} {}
};

template <>
struct fmt::formatter<Token> {
  constexpr auto parse(fmt::format_parse_context& ctx)
      -> decltype(ctx.begin()) {
    return ctx.end();
  }

  template <typename FormatContext>
  auto format(const Token& token, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    // For some reason I can't use overload visit pattern with `ctx.out` here
    switch (token.type) {
      case IDENTIFIER:
        return fmt::format_to(ctx.out(), "({} '{}')", tt_to_string(token.type),
                              token.lexeme);
      case STRING:
        return fmt::format_to(ctx.out(), "({} '{}')", tt_to_string(token.type),
                              std::get<std::string>(token.literal.value()));
      case NUMBER:
        return fmt::format_to(ctx.out(), "({} '{}')", tt_to_string(token.type),
                              std::get<double>(token.literal.value()));
      default:
        return fmt::format_to(ctx.out(), "({})", tt_to_string(token.type));
    }
  }
};

#endif  //LOXALONE_TOKENS_H

//
// Created by Htet Aung Shine on 30/11/22.
//

#ifndef LOXALONE_PARSER_H
#define LOXALONE_PARSER_H

#include <optional>
#include <vector>
#include "Error.h"
#include "Expr.h"
#include "Token.h"

class ParserError : std::exception {
 public:
  const Token& token;
  const std::string msg;

  ParserError(const Token& token, std::string&& msg)
      : token{token}, msg{std::move(msg)} {}
};

class Parser {
 public:
  explicit Parser(const std::vector<Token>& tokens) : tokens_m{tokens} {}

  auto parse() -> std::optional<Expr>;

 private:
  auto expression() -> Expr;
  auto equality() -> Expr;
  auto comparison() -> Expr;
  auto term() -> Expr;
  auto factor() -> Expr;
  auto unary() -> Expr;
  auto primary() -> Expr;
  auto previous() -> const Token&;

  auto check(TokenType) -> bool;
  auto advance() -> const Token&;
  auto peek() -> const Token&;
  auto is_at_end() -> bool;

  template <typename Head, typename... Tail>
  requires std::same_as<Head, TokenType> auto match(Head, Tail...) -> bool;
  auto match() -> bool;

  auto consume(TokenType, std::string_view&&) -> Token;
  auto parser_error(const Token& token, const std::string_view& err)
      -> ParserError;

  auto synchronize() -> void;

  const std::vector<Token>& tokens_m;
  int current_m = 0;
};

#endif  //LOXALONE_PARSER_H

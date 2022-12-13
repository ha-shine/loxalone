//
// Created by Htet Aung Shine on 30/11/22.
//

#ifndef LOXALONE_PARSER_H
#define LOXALONE_PARSER_H

#include <optional>
#include <vector>
#include "Error.h"
#include "Expr.h"
#include "Stmt.h"
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

  // The main entry point of the parser,
  // parses the source text into list of statements
  auto parse() -> std::vector<Stmt>;

 private:

  // Statement parsing functions
  auto declaration() -> Stmt;
  auto var_declaration() -> Stmt;
  auto statement() -> Stmt;
  auto if_statement() -> Stmt;
  auto print_statement() -> Stmt;
  auto while_statement() -> Stmt;
  auto for_statement() -> Stmt;
  auto expression_statement() -> Stmt;

  // This returns a list of block instead of statement because this
  // will be reused in other parsing, e.g. functions.
  auto block() -> std::vector<Stmt>;

  // Expression parsing functions
  auto expression() -> Expr;
  auto assignment() -> Expr;
  auto or_expression() -> Expr;
  auto and_expression() -> Expr;
  auto equality() -> Expr;
  auto comparison() -> Expr;
  auto term() -> Expr;
  auto factor() -> Expr;
  auto unary() -> Expr;
  auto primary() -> Expr;

  // Helper methods to help with the parsing
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

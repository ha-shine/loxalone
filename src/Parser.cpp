//
// Created by Htet Aung Shine on 30/11/22.
//

#include "Parser.h"

auto Parser::parse() -> std::optional<Expr> {
  try {
    return expression();
  } catch (const ParserError& pe) {
    return std::nullopt;
  }
}

auto Parser::expression() -> Expr {
  return equality();
}

auto Parser::equality() -> Expr {
  Expr expr = comparison();

  while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
    Token oper = previous();
    Expr right = comparison();
    expr = Expr{std::make_unique<BinaryExpr>(std::move(expr), std::move(oper),
                                             std::move(right))};
  }

  return expr;
}

auto Parser::comparison() -> Expr {
  Expr expr = term();

  while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
    Token oper = previous();
    Expr right = term();
    expr = Expr{std::make_unique<BinaryExpr>(std::move(expr), std::move(oper),
                                             std::move(right))};
  }

  return expr;
}

auto Parser::term() -> Expr {
  Expr expr = factor();

  while (match(TokenType::PLUS, TokenType::MINUS)) {
    Token oper = previous();
    Expr right = factor();
    expr = Expr{std::make_unique<BinaryExpr>(std::move(expr), std::move(oper),
                                             std::move(right))};
  }

  return expr;
}

auto Parser::factor() -> Expr {
  Expr expr = unary();

  while (match(TokenType::SLASH, TokenType::STAR)) {
    Token oper = previous();
    Expr right = unary();
    expr = Expr{std::make_unique<BinaryExpr>(std::move(expr), std::move(oper),
                                             std::move(right))};
  }

  return expr;
}

auto Parser::unary() -> Expr {
  if (match(TokenType::BANG, TokenType::MINUS)) {
    Token oper = previous();
    Expr right = unary();
    return Expr{std::make_unique<UnaryExpr>(std::move(oper), std::move(right))};
  }

  return primary();
}

auto Parser::primary() -> Expr {
  if (match(TokenType::TRUE))
    return Expr{std::make_unique<LiteralVal>(true)};
  if (match(TokenType::FALSE))
    return Expr{std::make_unique<LiteralVal>(false)};
  if (match(TokenType::NIL))
    return Expr{std::make_unique<LiteralVal>(std::monostate{})};
  if (match(TokenType::NUMBER, TokenType::STRING))
    return Expr{
        std::make_unique<LiteralVal>(lox_literal{previous().literal.value()})};
  if (match(TokenType::LEFT_PAREN)) {
    Expr expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression");
    return Expr{std::make_unique<GroupingExpr>(std::move(expr))};
  }

  throw parser_error(peek(), "Expect expression.");
}

auto Parser::previous() -> const Token& {
  return tokens_m[current_m - 1];
}

auto Parser::check(TokenType type) -> bool {
  if (is_at_end())
    return false;
  return peek().type == type;
}

auto Parser::advance() -> const Token& {
  if (!is_at_end())
    current_m++;
  return previous();
}

auto Parser::peek() -> const Token& {
  return tokens_m[current_m];
}

auto Parser::is_at_end() -> bool {
  return peek().type == TokenType::EOF;
}

template <typename Head, typename... Tail>
requires std::same_as<Head, TokenType> auto Parser::match(Head head,
                                                          Tail... tail)
    -> bool {
  if (check(head)) {
    advance();
    return true;
  }

  return match(tail...);
}

auto Parser::match() -> bool {
  return false;
}

auto Parser::consume(TokenType type, std::string_view&& msg) -> Token {
  if (check(type))
    return advance();
  throw parser_error(peek(), msg);
}

auto Parser::parser_error(const Token& token, const std::string_view& err) -> ParserError {
  if (token.type == TokenType::EOF) {
    report(token.line, " at end", err);
  } else {
    report(token.line, fmt::format(" at '{}'", token.lexeme), err);
  }
  return ParserError{token, std::string{err}};
}

auto Parser::synchronize() -> void {
  advance();

  while (!is_at_end()) {
    if (previous().type == TokenType::SEMICOLON)
      return;

    switch (peek().type) {
      case TokenType::CLASS:
      case TokenType::FUN:
      case TokenType::VAR:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::WHILE:
      case TokenType::PRINT:
      case TokenType::RETURN:
        return;
      default:
        advance();
    }
  }
}

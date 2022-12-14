//
// Created by Htet Aung Shine on 30/11/22.
//

#include "Parser.h"

namespace loxalone {

auto Parser::parse() -> std::vector<Stmt> {
  std::vector<Stmt> result{};
  try {
    while (!is_at_end()) {
      result.emplace_back(declaration());
    }

    return result;
  } catch (const ParserError& err) {
    // TODO: How do we report this?
    return {};
  }
}

auto Parser::declaration() -> Stmt {
  try {
    if (match(TokenType::CLASS)) return class_declaration();
    if (match(TokenType::FUN)) return function("function");
    if (match(TokenType::VAR)) return var_declaration();

    return statement();
  } catch (const ParserError& err) {
    synchronize();
    return Var::empty();
  }
}

auto Parser::class_declaration() -> Stmt {
  Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
  consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

  std::vector<FunctionPtr> methods{};
  while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
    methods.emplace_back(std::get<FunctionPtr>(function("method")));
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after class body");
  return Class::create(std::move(name), std::move(methods));
}

auto Parser::var_declaration() -> Stmt {
  Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

  Expr init = Variable::empty();
  if (match(TokenType::EQUAL)) {
    init = expression();
  }

  consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
  return Var::create(std::move(name), std::move(init));
}

auto Parser::statement() -> Stmt {
  if (match(TokenType::IF)) return if_statement();
  if (match(TokenType::PRINT)) return print_statement();
  if (match(TokenType::RETURN)) return return_statement();
  if (match(TokenType::WHILE)) return while_statement();
  if (match(TokenType::FOR)) return for_statement();
  if (match(TokenType::LEFT_BRACE)) return Block::create(block());

  return expression_statement();
}

auto Parser::if_statement() -> Stmt {
  const Token& token = previous();
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  Expr condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

  Stmt then_branch = statement();
  Stmt else_branch = Block::empty();
  if (match(TokenType::ELSE)) else_branch = statement();

  return If::create(std::move(condition), Token{token}, std::move(then_branch),
                    std::move(else_branch));
}

auto Parser::print_statement() -> Stmt {
  Expr expr = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after value.");
  return Print::create(std::move(expr));
}

auto Parser::while_statement() -> Stmt {
  const Token& token = previous();
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  Expr condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
  Stmt body = statement();
  return While::create(std::move(condition), std::move(body), Token{token});
}

auto Parser::for_statement() -> Stmt {
  const Token& token = previous();

  consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
  Stmt initializer{};
  if (match(TokenType::SEMICOLON)) {
    initializer = Var::empty();
  } else if (match(TokenType::VAR)) {
    initializer = var_declaration();
  } else {
    initializer = expression_statement();
  }

  Expr condition{};
  if (!check(TokenType::SEMICOLON)) {
    condition = expression();
  }
  consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

  Expr increment{};
  if (!check(TokenType::RIGHT_PAREN)) {
    increment = expression();
  }
  consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

  // The body of the for loop
  std::vector<Stmt> stmts{};
  stmts.emplace_back(statement());

  // If the increment expression is not null, add it to the end of body
  if (!expr_is_null(increment))
    stmts.emplace_back(Expression::create(std::move(increment)));

  // The for loop's body is a list of statement with increment expression
  // added at the end
  Stmt body = Block::create(std::move(stmts));

  // If the condition expression is null, use `true` literal (an infinite
  // loop)
  if (expr_is_null(condition)) condition = Literal::create(true);

  // De-sugar for loop into a while loop with the condition.
  body = While::create(std::move(condition), std::move(body), Token{token});

  // The initializer needs to be executed first before looping
  if (!stmt_is_null(initializer)) {
    std::vector<Stmt> block{};
    block.emplace_back(std::move(initializer));
    block.emplace_back(std::move(body));
    body = Block::create(std::move(block));
  }

  return body;
}

auto Parser::expression_statement() -> Stmt {
  Expr expr = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return Expression::create(std::move(expr));
}

auto Parser::block() -> std::vector<Stmt> {
  std::vector<Stmt> statements{};
  while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
    statements.emplace_back(declaration());
  }
  consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return statements;
}

auto Parser::function(const std::string_view& kind) -> Stmt {
  Token name =
      consume(TokenType::IDENTIFIER, fmt::format("Expect {} name.", kind));
  consume(TokenType::LEFT_PAREN,
          fmt::format("Expect '(' after {} name.", kind));

  std::vector<Token> params{};
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (params.size() >= 255) {
        parser_error(peek(), "Can't have more than 255 parameters.");
      }

      params.emplace_back(
          consume(TokenType::IDENTIFIER, "Expect parameter name."));
    } while (match(TokenType::COMMA));
  }

  consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
  consume(TokenType::LEFT_BRACE,
          fmt::format("Expect '{{' before {} body.", kind));
  std::vector<Stmt> body{block()};

  return Function::create(std::move(name), std::move(params), std::move(body));
}

auto Parser::return_statement() -> Stmt {
  Token keyword = previous();
  Expr value = Variable::empty();
  if (!check(TokenType::SEMICOLON)) {
    value = expression();
  }

  consume(TokenType::SEMICOLON, "Expect ';' after return value.");
  return Return::create(std::move(keyword), std::move(value));
}

auto Parser::expression() -> Expr { return assignment(); }

auto Parser::assignment() -> Expr {
  Expr expr = or_expression();

  if (match(TokenType::EQUAL)) {
    const Token& equals = previous();
    Expr value = assignment();

    if (std::holds_alternative<VariablePtr>(expr)) {
      Token name = std::get<VariablePtr>(expr)->name_m;
      return Assign::create(std::move(name), std::move(value));
    }

    parser_error(equals, "Invalid assignment target.");
  }

  return expr;
}

auto Parser::or_expression() -> Expr {
  Expr expr = and_expression();
  while (match(TokenType::OR)) {
    const Token& oper = previous();
    Expr right = and_expression();
    expr = Logical::create(std::move(expr), Token{oper}, std::move(right));
  }
  return expr;
}

auto Parser::and_expression() -> Expr {
  Expr expr = equality();
  while (match(TokenType::AND)) {
    const Token& oper = previous();
    Expr right = equality();
    expr = Logical::create(std::move(expr), Token{oper}, std::move(right));
  }
  return expr;
}

auto Parser::equality() -> Expr {
  Expr expr = comparison();

  while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
    Token oper = previous();
    Expr right = comparison();
    expr = Binary::create(std::move(expr), std::move(oper), std::move(right));
  }

  return expr;
}

auto Parser::comparison() -> Expr {
  Expr expr = term();

  while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
               TokenType::LESS_EQUAL)) {
    Token oper = previous();
    Expr right = term();
    expr = Binary::create(std::move(expr), std::move(oper), std::move(right));
  }

  return expr;
}

auto Parser::term() -> Expr {
  Expr expr = factor();

  while (match(TokenType::PLUS, TokenType::MINUS)) {
    Token oper = previous();
    Expr right = factor();
    expr = Binary::create(std::move(expr), std::move(oper), std::move(right));
  }

  return expr;
}

auto Parser::factor() -> Expr {
  Expr expr = unary();

  while (match(TokenType::SLASH, TokenType::STAR)) {
    Token oper = previous();
    Expr right = unary();
    expr = Binary::create(std::move(expr), std::move(oper), std::move(right));
  }

  return expr;
}

auto Parser::unary() -> Expr {
  if (match(TokenType::BANG, TokenType::MINUS)) {
    Token oper = previous();
    Expr right = unary();
    return Unary::create(std::move(oper), std::move(right));
  }

  return call();
}

auto Parser::call() -> Expr {
  Expr expr = primary();

  while (true) {
    if (match(TokenType::LEFT_PAREN)) {
      expr = finish_call(std::move(expr));
    } else {
      break;
    }
  }

  return expr;
}

auto Parser::finish_call(Expr&& callee) -> Expr {
  std::vector<Expr> args{};
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (args.size() >= 255) {
        throw parser_error(peek(), "Can't have more than 255 arguments.");
      }
      args.emplace_back(expression());
    } while (match(TokenType::COMMA));
  }

  const Token& paren =
      consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
  return Call::create(std::move(callee), Token{paren}, std::move(args));
}

auto Parser::primary() -> Expr {
  if (match(TokenType::TRUE)) return Literal::create(true);
  if (match(TokenType::FALSE)) return Literal::create(false);
  if (match(TokenType::NIL)) return Literal::create(std::monostate{});
  if (match(TokenType::NUMBER, TokenType::STRING))
    return Literal::create(lox_literal{previous().literal.value()});
  if (match(TokenType::LEFT_PAREN)) {
    Expr expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression");
    return Grouping::create(std::move(expr));
  }
  if (match(TokenType::IDENTIFIER)) {
    Token prev = previous();
    return Variable::create(std::move(prev));
  }

  throw parser_error(peek(), "Expect expression.");
}

auto Parser::previous() -> const Token& { return tokens_m[current_m - 1]; }

auto Parser::check(TokenType type) -> bool {
  if (is_at_end()) return false;
  return peek().type == type;
}

auto Parser::advance() -> const Token& {
  if (!is_at_end()) current_m++;
  return previous();
}

auto Parser::peek() -> const Token& { return tokens_m[current_m]; }

auto Parser::is_at_end() -> bool { return peek().type == TokenType::EOF_; }

template <typename Head, typename... Tail>
  requires std::same_as<Head, TokenType>
auto Parser::match(Head head, Tail... tail) -> bool {
  if (check(head)) {
    advance();
    return true;
  }

  return match(tail...);
}

auto Parser::match() -> bool { return false; }

auto Parser::consume(TokenType type, std::string_view&& msg) -> Token {
  if (check(type)) return advance();
  throw parser_error(peek(), msg);
}

auto Parser::parser_error(const Token& token, const std::string_view& err)
    -> ParserError {
  if (token.type == TokenType::EOF_) {
    report(token.line, " at end", err);
  } else {
    report(token.line, fmt::format(" at '{}'", token.lexeme), err);
  }
  return ParserError{token, std::string{err}};
}

auto Parser::synchronize() -> void {
  advance();

  while (!is_at_end()) {
    if (previous().type == TokenType::SEMICOLON) return;

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

}  // namespace loxalone
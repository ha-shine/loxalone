//
// Created by Htet Aung Shine on 30/11/22.
//

#include "Parser.h"

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
    if (match(TokenType::FUN))
      return function("function");
    if (match(TokenType::VAR))
      return var_declaration();

    return statement();
  } catch (const ParserError& err) {
    synchronize();
    return std::unique_ptr<Var>(nullptr);
  }
}

auto Parser::var_declaration() -> Stmt {
  Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

  Expr init = std::unique_ptr<Variable>(nullptr);
  if (match(TokenType::EQUAL)) {
    init = expression();
  }

  consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
  return Stmt{std::make_unique<Var>(std::move(name), std::move(init))};
}

auto Parser::statement() -> Stmt {
  if (match(TokenType::IF))
    return if_statement();
  if (match(TokenType::PRINT))
    return print_statement();
  if (match(TokenType::WHILE))
    return while_statement();
  if (match(TokenType::FOR))
    return for_statement();
  if (match(TokenType::LEFT_BRACE))
    return Stmt{std::make_unique<Block>(block())};

  return expression_statement();
}

auto Parser::if_statement() -> Stmt {
  const Token& token = previous();
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  Expr condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

  Stmt then_branch = statement();
  Stmt else_branch = Stmt{std::unique_ptr<Block>(nullptr)};
  if (match(TokenType::ELSE))
    else_branch = statement();

  return Stmt{std::make_unique<If>(std::move(condition), Token{token},
                                   std::move(then_branch),
                                   std::move(else_branch))};
}

auto Parser::print_statement() -> Stmt {
  Expr expr = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after value.");
  return Stmt{std::make_unique<Print>(std::move(expr))};
}

auto Parser::while_statement() -> Stmt {
  const Token& token = previous();
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  Expr condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
  Stmt body = statement();
  return Stmt{std::make_unique<While>(std::move(condition), std::move(body),
                                      Token{token})};
}

auto Parser::for_statement() -> Stmt {
  const Token& token = previous();

  consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
  Stmt initializer{};
  if (match(TokenType::SEMICOLON)) {
    initializer = Stmt{std::unique_ptr<Var>(nullptr)};
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
    stmts.emplace_back(std::make_unique<Expression>(std::move(increment)));

  // The for loop's body is a list of statement with increment expression
  // added at the end
  Stmt body = Stmt{std::make_unique<Block>(std::move(stmts))};

  // If the condition expression is null, use `true` literal (an infinite loop)
  if (expr_is_null(condition))
    condition = Expr{std::make_unique<Literal>(true)};

  // De-sugar for loop into a while loop with the condition.
  body = Stmt{std::make_unique<While>(std::move(condition), std::move(body),
                                      Token{token})};

  // The initializer needs to be executed first before looping
  if (!stmt_is_null(initializer)) {
    std::vector<Stmt> block{};
    block.emplace_back(std::move(initializer));
    block.emplace_back(std::move(body));
    body = Stmt{std::make_unique<Block>(std::move(block))};
  }

  return body;
}

auto Parser::expression_statement() -> Stmt {
  Expr expr = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return Stmt{std::make_unique<Expression>(std::move(expr))};
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
  Token name = consume(TokenType::IDENTIFIER, fmt::format("Expect {} name.", kind));
  consume(TokenType::LEFT_PAREN, fmt::format("Expect '(' after {} name.", kind));

  std::vector<Token> params{};
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (params.size() >= 255) {
        parser_error(peek(), "Can't have more than 255 parameters.");
      }

      params.emplace_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
    } while (match(TokenType::COMMA));
  }

  consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
  consume(TokenType::LEFT_BRACE, fmt::format("Expect '{{' before {} body.", kind));
  std::vector<Stmt> body{block()};

  return Stmt{std::make_unique<Function>(std::move(name), std::move(params), std::move(body))};
}

auto Parser::expression() -> Expr {
  return assignment();
}

auto Parser::assignment() -> Expr {
  Expr expr = or_expression();

  if (match(TokenType::EQUAL)) {
    const Token& equals = previous();
    Expr value = assignment();

    if (std::holds_alternative<VariablePtr>(expr)) {
      Token name = std::get<VariablePtr>(expr)->name_m;
      return Expr{std::make_unique<Assign>(std::move(name), std::move(value))};
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
    expr = Expr{std::make_unique<Logical>(std::move(expr), Token{oper},
                                          std::move(right))};
  }
  return expr;
}

auto Parser::and_expression() -> Expr {
  Expr expr = equality();
  while (match(TokenType::AND)) {
    const Token& oper = previous();
    Expr right = equality();
    expr = Expr{std::make_unique<Logical>(std::move(expr), Token{oper},
                                          std::move(right))};
  }
  return expr;
}

auto Parser::equality() -> Expr {
  Expr expr = comparison();

  while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
    Token oper = previous();
    Expr right = comparison();
    expr = Expr{std::make_unique<Binary>(std::move(expr), std::move(oper),
                                             std::move(right))};
  }

  return expr;
}

auto Parser::comparison() -> Expr {
  Expr expr = term();

  while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
               TokenType::LESS_EQUAL)) {
    Token oper = previous();
    Expr right = term();
    expr = Expr{std::make_unique<Binary>(std::move(expr), std::move(oper),
                                             std::move(right))};
  }

  return expr;
}

auto Parser::term() -> Expr {
  Expr expr = factor();

  while (match(TokenType::PLUS, TokenType::MINUS)) {
    Token oper = previous();
    Expr right = factor();
    expr = Expr{std::make_unique<Binary>(std::move(expr), std::move(oper),
                                             std::move(right))};
  }

  return expr;
}

auto Parser::factor() -> Expr {
  Expr expr = unary();

  while (match(TokenType::SLASH, TokenType::STAR)) {
    Token oper = previous();
    Expr right = unary();
    expr = Expr{std::make_unique<Binary>(std::move(expr), std::move(oper),
                                             std::move(right))};
  }

  return expr;
}

auto Parser::unary() -> Expr {
  if (match(TokenType::BANG, TokenType::MINUS)) {
    Token oper = previous();
    Expr right = unary();
    return Expr{std::make_unique<Unary>(std::move(oper), std::move(right))};
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

  const Token& paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
  return Expr{std::make_unique<Call>(std::move(callee), Token{paren}, std::move(args))};
}

auto Parser::primary() -> Expr {
  if (match(TokenType::TRUE))
    return Expr{std::make_unique<Literal>(true)};
  if (match(TokenType::FALSE))
    return Expr{std::make_unique<Literal>(false)};
  if (match(TokenType::NIL))
    return Expr{std::make_unique<Literal>(std::monostate{})};
  if (match(TokenType::NUMBER, TokenType::STRING))
    return Expr{
        std::make_unique<Literal>(lox_literal{previous().literal.value()})};
  if (match(TokenType::LEFT_PAREN)) {
    Expr expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression");
    return Expr{std::make_unique<Grouping>(std::move(expr))};
  }
  if (match(TokenType::IDENTIFIER)) {
    Token prev = previous();
    return Expr{std::make_unique<Variable>(std::move(prev))};
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
  return peek().type == TokenType::EOF_;
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

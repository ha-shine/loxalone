//
// Created by Htet Aung Shine on 22/11/2022.
//

#include <string>

#include "errors.h"
#include "scanner.h"

auto is_digit(char ch) -> bool {
  return ch >= '0' && ch <= '9';
}

auto Scanner::scan_tokens() -> std::vector<Token> {
  while (!is_at_end()) {
    start = current;
    scan_token();
  }

  tokens.emplace_back(EOF_CHAR, "", std::nullopt, line);
  return tokens;
}

// TODO: Implement
auto Scanner::scan_token() -> void {
  char c = advance();

  switch (c) {
    case '(':
      add_token(LEFT_PAREN);
      break;
    case ')':
      add_token(RIGHT_PAREN);
      break;
    case '{':
      add_token(LEFT_BRACE);
      break;
    case '}':
      add_token(RIGHT_BRACE);
      break;
    case ',':
      add_token(COMMA);
      break;
    case '.':
      add_token(DOT);
      break;
    case '-':
      add_token(MINUS);
      break;
    case '+':
      add_token(PLUS);
      break;
    case ';':
      add_token(SEMICOLON);
      break;
    case '*':
      add_token(STAR);
      break;
    case '!':
      add_token(match('=') ? BANG_EQUAL : BANG);
      break;
    case '=':
      add_token(match('=') ? EQUAL_EQUAL : EQUAL);
      break;
    case '<':
      add_token(match('=') ? LESS_EQUAL : LESS);
      break;
    case '>':
      add_token(match('=') ? GREATER_EQUAL : GREATER);
      break;
    case '/':
      if (match('/')) {
        while (peek() != '\n' && !is_at_end())
          advance();
      } else {
        add_token(SLASH);
      }
      break;
    case '"':
      string();
      break;

    // Whitespaces
    case ' ':
    case '\t':
    case '\r':
      break;
    case '\n':
      line++;
      break;
    default:
      // TODO: This should set an error flag in Lox so we won't execute
      //       the malformed program though we can just return empty tokens or
      //       result from top-level scan_tokens()?
      if (is_digit(c)) {
        number();
      } else {
        error(line, "Unexpected character.");
      }
  }
}

auto Scanner::is_at_end() -> bool {
  return current >= source.length();
}

// TODO: Probably not char if we want unicode
auto Scanner::advance() -> char {
  return source[current++];
}

auto Scanner::match(char c) -> bool {
  if (is_at_end())
    return false;
  if (source[current] != c)
    return false;

  current++;
  return true;
}

auto Scanner::peek() -> char {
  if (is_at_end())
    return '\0';
  return source[current];
}

auto Scanner::peek_next() -> char {
  if (current + 1 >= source.length())
    return '\0';
  return source[current + 1];
}

auto Scanner::string() -> void {
  while (peek() != '"' && !is_at_end()) {
    if (peek() == '\n')
      line++;
    advance();
  }

  if (is_at_end()) {
    error(line, "Unterminated string.");
    return;
  }

  advance();  // consume the closing "
  std::string value{source.begin() + start + 1, source.begin() + current - 1};
  add_token(STRING, std::string{source.begin() + start + 1,
                                source.begin() + current - 1});
}

auto Scanner::number() -> void {
  while (is_digit(peek()))
    advance();

  if (peek() == '.' && is_digit(peek_next())) {
    advance();  // consume the "."

    while (is_digit(peek()))
      advance();
  }

  add_token(NUMBER, std::stod(std::string{source.begin() + start,
                                          source.begin() + current}));
}

auto Scanner::add_token(TokenType type) -> void {
  add_token(type, std::nullopt);
}

auto Scanner::add_token(TokenType type, std::optional<lox_literal>&& literal)
    -> void {
  tokens.emplace_back(
      type, std::string{source.begin() + start, source.begin() + current},
      std::move(literal), line);
}

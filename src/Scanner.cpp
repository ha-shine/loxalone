//
// Created by Htet Aung Shine on 22/11/2022.
//

#include <string>
#include <unordered_map>

#include "Error.h"
#include "Scanner.h"

static std::unordered_map<std::string, TokenType> keywords{
    {"and", TokenType::AND},   {"class", TokenType::CLASS}, {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
    {"for", TokenType::FOR},   {"fun", TokenType::FUN},     {"if", TokenType::IF},         {"nil", TokenType::NIL},
    {"or", TokenType::OR},     {"print", TokenType::PRINT}, {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
    {"this", TokenType::THIS}, {"true", TokenType::TRUE},   {"var", TokenType::VAR},       {"while", TokenType::WHILE}};

auto is_digit(char ch) -> bool {
  return ch >= '0' && ch <= '9';
}

auto is_alpha(char ch) -> bool {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_');
}

auto is_alphanumeric(char ch) -> bool {
  return is_alpha(ch) || is_digit(ch);
}

auto Scanner::scan_tokens() -> std::optional<std::vector<Token>> {
  while (!is_at_end()) {
    start_m = current_m;
    scan_token();
  }

  tokens.emplace_back(TokenType::EOF, "", std::nullopt, line_m);
  return well_formed_m ? std::optional{tokens} : std::nullopt;
}

// TODO: Implement
auto Scanner::scan_token() -> void {
  char c = advance();

  switch (c) {
    case '(':
      add_token(TokenType::LEFT_PAREN);
      break;
    case ')':
      add_token(TokenType::RIGHT_PAREN);
      break;
    case '{':
      add_token(TokenType::LEFT_BRACE);
      break;
    case '}':
      add_token(TokenType::RIGHT_BRACE);
      break;
    case ',':
      add_token(TokenType::COMMA);
      break;
    case '.':
      add_token(TokenType::DOT);
      break;
    case '-':
      add_token(TokenType::MINUS);
      break;
    case '+':
      add_token(TokenType::PLUS);
      break;
    case ';':
      add_token(TokenType::SEMICOLON);
      break;
    case '*':
      add_token(TokenType::STAR);
      break;
    case '!':
      add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      break;
    case '=':
      add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      break;
    case '<':
      add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      break;
    case '>':
      add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    case '/':
      if (match('/')) {
        while (peek() != '\n' && !is_at_end())
          advance();
      } else if (match('*')) {
        bool done = false;
        while (peek() != '\n' && !is_at_end() && !done) {
          char now = advance();
          if (now == '*' && peek() == '/') {
            advance(); // consume "/"
            done = true;
          }
        }

        // done = comment was well-formed, scanning is done
        // if done is false, we reached the end without closing the comment
        if (done)
          break;

        scanner_error(line_m, "Unterminated block comment.");
      } else {
        add_token(TokenType::SLASH);
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
      line_m++;
      break;
    default:
      // TODO: This should set an parser_error flag in Lox so we won't execute
      //       the malformed program though we can just return empty tokens_m or
      //       result from top-level scan_tokens()?
      if (is_digit(c)) {
        number();
      } else if (is_alpha(c)) {
        identifier();
      } else {
        scanner_error(line_m, "Unexpected character.");
      }
  }
}

auto Scanner::is_at_end() -> bool {
  return current_m >= source.length();
}

// TODO: Probably not char if we want unicode
auto Scanner::advance() -> char {
  return source[current_m++];
}

auto Scanner::match(char c) -> bool {
  if (is_at_end())
    return false;
  if (source[current_m] != c)
    return false;

  current_m++;
  return true;
}

auto Scanner::peek() -> char {
  if (is_at_end())
    return '\0';
  return source[current_m];
}

auto Scanner::peek_next() -> char {
  if (current_m + 1 >= source.length())
    return '\0';
  return source[current_m + 1];
}

auto Scanner::string() -> void {
  while (peek() != '"' && !is_at_end()) {
    if (peek() == '\n')
      line_m++;
    advance();
  }

  if (is_at_end()) {
    scanner_error(line_m, "Unterminated string.");
    return;
  }

  advance();  // consume the closing "
  std::string value{source.begin() + start_m + 1, source.begin() + current_m - 1};
  add_token(TokenType::STRING, std::string{source.begin() + start_m + 1,
                                source.begin() + current_m - 1});
}

auto Scanner::number() -> void {
  while (is_digit(peek()))
    advance();

  if (peek() == '.' && is_digit(peek_next())) {
    advance();  // consume the "."

    while (is_digit(peek()))
      advance();
  }

  add_token(TokenType::NUMBER, std::stod(std::string{source.begin() + start_m,
                                          source.begin() + current_m}));
}

auto Scanner::identifier() -> void {
  while (is_alphanumeric(peek()))
    advance();

  std::string text = std::string{source.begin()+ start_m, source.begin()+ current_m};
  auto result = keywords.find(text);
  if (result != keywords.end())
    add_token(result->second);
  else
    add_token(TokenType::IDENTIFIER);
}

auto Scanner::add_token(TokenType type) -> void {
  add_token(type, std::nullopt);
}

auto Scanner::add_token(TokenType type, std::optional<lox_literal>&& literal)
    -> void {
  tokens.emplace_back(
      type, std::string{source.begin() + start_m, source.begin() + current_m},
      std::move(literal), line_m);
}

auto Scanner::scanner_error(int line, const std::string_view& msg) -> void {
  error(line, msg);
  well_formed_m = false;
}

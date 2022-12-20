//
// Created by Htet Aung Shine on 24/11/22.
//

#ifndef LOXALONE_MISC_H
#define LOXALONE_MISC_H

#include <string>
#include <vector>

namespace loxalone {
// TODO: Find if there are standard library functions for these
// TODO: Integrate with a testing framework

// Transform all the letters from the original string using the given
// function parameter
static auto transform_chars(const std::string_view& source,
                            const std::function<char(char)>& fun)
    -> std::string {
  std::string result{};
  for (const auto& ch : source) result.push_back(static_cast<char>(fun(ch)));
  return result;
}

// Returns a new string with all the letters from the original string in
// lowercase (or uppercase)
auto to_lowercase(const std::string_view&) -> std::string;
auto to_uppercase(const std::string_view&) -> std::string;

// Split the original string into multiple parts using the given delimiter.
// Returns a list of string views that are tied to the original.
auto split(const std::string_view&, char) -> std::vector<std::string_view>;

// Returns the view with whitespaces removed from left and right
auto trim(const std::string_view&) -> std::string_view;
}  // namespace loxalone

#endif  // LOXALONE_MISC_H

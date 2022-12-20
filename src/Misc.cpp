//
// Created by Htet Aung Shine on 24/11/22.
//

#include "Misc.h"

namespace loxalone {
auto to_lowercase(const std::string_view& source) -> std::string {
  return transform_chars(source, tolower);
}

auto to_uppercase(const std::string_view& source) -> std::string {
  return transform_chars(source, toupper);
}

auto split(const std::string_view& source, char delim)
    -> std::vector<std::string_view> {
  std::vector<std::string_view> result{};

  auto prev = source.begin();
  auto iter = std::find(prev, source.end(), delim);

  while (iter != source.end()) {
    result.emplace_back(prev, iter - prev);
    prev = iter + 1;
    iter = std::find(prev, source.end(), delim);
  }

  // If the last call to `find` did not find any delimiter, we need to add
  // that part into the result.
  if (iter - prev > 0) result.emplace_back(prev, iter - prev);

  return result;
}

auto trim(const std::string_view& source) -> std::string_view {
  auto begin = source.begin();
  while (isspace(*begin)) begin++;

  auto end = source.end() - 1;
  while (isspace(*end)) end--;

  return {begin, static_cast<std::string_view::size_type>(end - begin + 1)};
}
}  // namespace loxalone
//
// Created by Htet Aung Shine on 24/11/22.
//

#ifndef LOXALONE_MISC_H
#define LOXALONE_MISC_H

#include <string>
#include <vector>

// TODO: Find if there are standard library functions for these
// TODO: Integrate with a testing framework

// Returns a new string with all the letters from the original string in
// lowercase
auto to_lowercase(const std::string_view&) -> std::string;

// Split the original string into multiple parts using the given delimiter.
// Returns a list of string views that are tied to the original.
auto split(const std::string_view&, char) -> std::vector<std::string_view>;

// Returns the view with whitespaces removed from left and right
auto trim(const std::string_view&) -> std::string_view;

#endif  //LOXALONE_MISC_H

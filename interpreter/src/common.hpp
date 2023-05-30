#pragma once

#include <string>
#include <string_view>
#include <vector>

// contains common code & macros

constexpr bool DEBUG       = true;
constexpr auto VERSION     = "0.0.5";
constexpr auto LINE_SYMBOL = "\n~> ";

// terminal helpers
constexpr auto YELLOW = "\033[1;33m";
constexpr auto RED    = "\033[1;31m";
constexpr auto BLACK  = "\033[30m";
constexpr auto GREY   = "\033[37m";
constexpr auto ORANGE = "\033[33m";
constexpr auto PURPLE = "\033[35m";
constexpr auto BLUE   = "\033[34m";
constexpr auto GREEN  = "\033[32m";
constexpr auto CLEAR  = "\033[0m";

void print_error(std::string_view s);
void print_debug(std::string_view s);

// returns value from queue
template <class Q>
inline auto pop_front(Q& q) -> typename Q::value_type {
  auto front = q.front();
  q.erase(q.begin());
  return front;
}

// creates individual args from a string
auto args_splitter(const std::string str) -> std::vector<std::string>;

// compares strings in a case insensitive method
auto iequals(const std::string& a, const std::string& b) -> bool;

// case insensitive version stuff for unordered_map
// should i go nims route and be underscore insensitive too ?
struct imaphash {
  size_t operator()(const std::string& Keyval) const;
};

struct imapequal {
  bool operator()(const std::string& Left, const std::string& Right) const;
};

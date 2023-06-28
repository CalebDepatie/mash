#include "common.hpp"

#include <algorithm>
#include <iostream>

void print_error(std::string_view s) {
  std::cout << RED << "Error: " << s << CLEAR << std::endl;
}

void print_debug(std::string_view s) {
  if constexpr (DEBUG) { std::cout << YELLOW << "Debug: " << s << CLEAR << std::endl; }
}

auto args_splitter(const std::string str) -> std::vector<std::string> {
  using namespace std;
  vector<string> tokens;

  // had to replace getline for splitting to words
  // this now includes newlines as seperate 'words' and generally
  // does a better job of seperating words
  int pc = 0; // counter
  int ipc = 0; // initial counter
  bool comment_flag = false;

  while (pc < str.size()) {
    if (str[pc] == '#')
      comment_flag = true;

    if (comment_flag) {
      if ((str[pc] == '\r' && str[pc+1] == '\n') || str[pc] == '\n') {
        comment_flag = false;

        ipc = ++pc;
      }

      pc++;
      continue;
    }

    if (str[pc] == ' ') {
      if (pc - ipc > 0)
        tokens.emplace_back(str.substr(ipc, pc-ipc));

      ipc = pc+1;
    }

    if (str[pc] == '\r' && str[pc+1] == '\n') {
      if (pc - ipc > 0)
        tokens.emplace_back(str.substr(ipc, pc-ipc));

      tokens.emplace_back("\r\n");
      pc += 1;
      ipc = pc+1;
    }

    if (str[pc] == '\n') {
      if (pc - ipc > 0)
        tokens.emplace_back(str.substr(ipc, pc-ipc));

      tokens.emplace_back("\n");
      ipc = pc+1;
    }

    pc++;
  }

  if (pc - ipc > 0) {
    tokens.emplace_back(str.substr(ipc, pc-ipc));
    tokens.emplace_back("\n");
  }

  return tokens;
}

// todo: slated for removal after confirming no longer needed
auto iequals(const std::string& a, const std::string& b) -> bool {
  return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                    [](char a, char b) { return tolower(a) == tolower(b); });
}

size_t imaphash::operator()(const std::string& Keyval) const {
  const size_t m = 0x5bd1e995;
  size_t       h = 0;
  std::for_each(Keyval.begin(), Keyval.end(), [&](char c) { h += tolower(c); });
  // same hash mix libstdc++ does
  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;
  return h;
}

bool imapequal::operator()(const std::string& Left, const std::string& Right) const {
  return Left.size() == Right.size() &&
         std::equal(Left.begin(), Left.end(), Right.begin(),
                    [](char a, char b) { return tolower(a) == tolower(b); });
}

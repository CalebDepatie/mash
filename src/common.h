#pragma once

// contains common code & macros

#define VERSION "0.0.2"

#define ERROR(e) std::cout << "\033[1;31mError: " << e << "\033[0m" << std::endl;
#define DEBUG(s) std::cout << "\033[1;33mDebug: " << s << "\033[0m" << std::endl;

// returns value from queue
template<class Q>
inline auto pop_front(Q& q) -> typename Q::value_type {
  auto front = q.front();
  q.pop();
  return front;
}

// creates individual args from a string
auto args_splitter(const std::string str) -> std::queue<std::string>;

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

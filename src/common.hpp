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

// creates individual arg tokes from a string
auto tokenize(const std::string str) -> std::queue<std::string> {
  using namespace std;
  queue<string> tokens;
  stringstream str_stream(str);
  string arg;
  while(getline(str_stream, arg, ' '))
    tokens.emplace(arg);

  return tokens;
}

// compares strings in a case insensitive method
auto iequals(const std::string& a, const std::string& b) -> bool {
  return std::equal(a.begin(), a.end(),
                    b.begin(), b.end(),
                    [](char a, char b) {
                      return tolower(a) == tolower(b);
                    });
}

// case insensitive version stuff for unordered_map
// should i go nims route and be underscore insensitive too ?
struct imaphash {
    size_t operator()(const std::string& Keyval) const {
        const size_t m = 0x5bd1e995;
        size_t h = 0;
        std::for_each (Keyval.begin() , Keyval.end() , [&](char c) {
            h += tolower(c);
        });
        // same hash mix libstdc++ does
        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;
        return h;
    }
};

struct imapequal {
    bool operator()(const std::string& Left, const std::string& Right) const {
        return Left.size() == Right.size()
             && std::equal (Left.begin(), Left.end(), Right.begin(),
            [](char a , char b) {
            return tolower(a) == tolower(b);
        });
    }
};

#include "common.h"

auto args_splitter(const std::string str) -> std::vector<std::string> {
  using namespace std;
  vector<string> tokens;
  stringstream str_stream(str);
  string arg;
  while(getline(str_stream, arg, ' '))
    tokens.emplace_back(arg);

  return tokens;
}

auto iequals(const std::string& a, const std::string& b) -> bool {
  return std::equal(a.begin(), a.end(),
                    b.begin(), b.end(),
                    [](char a, char b) {
                      return tolower(a) == tolower(b);
                    });
}

size_t imaphash::operator()(const std::string& Keyval) const {
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

bool imapequal::operator()(const std::string& Left, const std::string& Right) const {
    return Left.size() == Right.size()
         && std::equal (Left.begin(), Left.end(), Right.begin(),
        [](char a , char b) {
        return tolower(a) == tolower(b);
    });
}

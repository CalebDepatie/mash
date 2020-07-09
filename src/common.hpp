#pragma once

// contains common code & macros

#define VERSION "0.0.2"

#define ERROR(e) std::cout << "\033[1;31mError: " << e << "\033[0m" << std::endl;

// creates individual arg tokes from a string
auto tokenize(const std::string str) -> std::vector<std::string> {
  using namespace std;
  vector<string> tokens;
  stringstream str_stream(str);
  string arg;
  while(getline(str_stream, arg, ' '))
    tokens.emplace_back(arg);

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
// TODO: make these better
struct ihash {
    size_t operator()(const std::string& Keyval) const {
        size_t h = 0;
        std::for_each( Keyval.begin() , Keyval.end() , [&](char c ) {
            h += tolower(c);
        });
        return h;
    }
};

struct iequal {
    bool operator()(const std::string& Left, const std::string& Right) const {
        return Left.size() == Right.size()
             && std::equal ( Left.begin() , Left.end() , Right.begin() ,
            []( char a , char b ) {
            return tolower(a) == tolower(b);
        });
    }
};

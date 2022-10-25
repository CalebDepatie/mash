#pragma once

#include <unordered_map>
#include <functional>

// header file containing the builtin commands
namespace token {
    class Token;
}

auto change_dir(std::vector<token::Token>& args) -> int;
auto quit(std::vector<token::Token>& args) -> int;
auto list_files(std::vector<token::Token>& args) -> int;

// using a hashtable like this allows for efficient name aliasing
inline std::unordered_map<std::string, std::function<int(std::vector<token::Token>&)>, imaphash, imapequal> builtins = {
  {"Change-Directory", &change_dir},
  {"cd", &change_dir},
  {"Quit", &quit},
  {"Exit", &quit},
  {"q", &quit},
  {"List-Files", &list_files},
  {"ls", &list_files},
};

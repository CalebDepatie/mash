#pragma once

#include <unordered_map>
#include <functional>

// header file containing the builtin commands
namespace token {
    class Token;
}

auto change_dir(std::queue<token::Token> args) -> int;
auto quit(std::queue<token::Token> args) -> int;
auto list_files(std::queue<token::Token> args) -> int;

// using a hashtable like this allows for efficient name aliasing
inline std::unordered_map<std::string, std::function<int(std::queue<token::Token>)>, imaphash, imapequal> builtins = {
  {"Change-Directory", &change_dir},
  {"cd", &change_dir},
  {"Quit", &quit},
  {"Exit", &quit},
  {"q", &quit},
  {"List-Files", &list_files},
  {"ls", &list_files},
};

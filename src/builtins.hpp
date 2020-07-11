#pragma once

#include <unordered_map>
#include <functional>

#include "common.hpp"

// header file containing the builtin commands

// TODO: switch from vector to a queue or a custom class?

auto change_dir(std::queue<std::string> args) -> int {
  if (args.size() == 0) {
    ERROR("expected argument to \"Change-Directory\"")
  } else {
    try {
      std::filesystem::current_path(std::filesystem::path(pop_front(args)));
    } catch (...) {
      ERROR("could not change directory")
    }
  }
  return 1;
}

auto quit(std::queue<std::string> args) -> int {
  return 0;
}

auto list_files(std::queue<std::string> args) -> int {
  namespace fs = std::filesystem;

  for (auto& f : fs::directory_iterator(".")) {
    if (f.is_directory()) {
      std::cout << "\033[34m";
    } else {
      std::cout << "\033[32m";
    }
    std::cout << f.path().filename().c_str() << " ";
  }

  std::cout << "\033[0m" << std::endl;

  return 1;
}

// using a hashtable like this allows for efficient name aliasing
std::unordered_map<std::string, std::function<int(std::queue<std::string>)>, imaphash, imapequal> builtins = {
  {"Change-Directory", &change_dir},
  {"cd", &change_dir},
  {"Quit", &quit},
  {"Exit", &quit},
  {"q", &quit},
  {"List-Files", &list_files},
  {"ls", &list_files},
};

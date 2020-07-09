#pragma once

#include <unordered_map>
#include <functional>

#include "common.hpp"

// header file containing the builtin commands

// TODO: switch from vector to a stack or a custom class?

auto change_dir(const std::vector<std::string> args) -> int {
  if (args.size() == 1) {
    ERROR("expected argument to \"Change-Directory\"")
  } else {
    try {
      std::filesystem::current_path(std::filesystem::path(args[1]));
    } catch (...) {
      ERROR("could not change directory")
    }
  }
  return 1;
}

auto quit(const std::vector<std::string> args) -> int {
  return 0;
}

// TODO: add optional colouring params
auto write_line(const std::vector<std::string> args) -> int {
  for (auto s : args)
    std::cout << s << " ";

  std::cout << "\033[0m" << std::endl;
  return 1;
}

auto list_files(const std::vector<std::string> args) -> int {
  namespace fs = std::filesystem;

  return 1;
}

// using a hashtable like this allows for efficient name aliasing
std::unordered_map<std::string, std::function<int(const std::vector<std::string>)>, ihash, iequal> builtins = {
  {"Change-Directory", &change_dir},
  {"cd", &change_dir},
  {"Quit", &quit},
  {"Exit", &quit},
  {"q", &quit},
  {"Write-Out", &write_line},
  {"Print", &write_line},
  {"List-Files", &list_files},
  {"ls", &list_files},
};

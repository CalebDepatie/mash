#pragma #ifndef

#include <unordered_map>
#include <functional>

#include "common.hpp"

// header file containing the builtin commands

inline auto change_dir(const std::vector<std::string> args) -> int {
  if (args.size() == 1) {
    ERROR("expected argument to \"Change-Directory\"")
  } else {
    std::filesystem::current_path(std::filesystem::path(args[1]));
  }
  return 1;
}

inline auto quit(const std::vector<std::string> args) -> int {
  return 0;
}

std::unordered_map<std::string, std::function<int(const std::vector<std::string>)>> builtins = {
  {"Change-Directory", &change_dir},
  {"cd", &change_dir},
  {"Quit", &quit},
  {"Exit", &quit},
  {"q", &quit},
};

#pragma #ifndef

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

// todo: aliasing
const std::string builtin_commands[] = {
  "Change-Directory",
  "Quit",
};

int (*builtin_funcs[]) (const std::vector<std::string>) = {
  &change_dir,
  &quit,
};

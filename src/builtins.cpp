#include "builtins.hpp"
#include "tokens.hpp"

auto change_dir(std::vector<token::Token>& args) -> int {
  if (args.size() == 0) {
    print_error("expected argument to \"Change-Directory\"");
  } else {
    try {
      std::filesystem::current_path(std::filesystem::path(pop_front(args).value));
    } catch (...) {
      print_error("could not change directory");
    }
  }
  return 1;
}

auto quit(std::vector<token::Token>& args) -> int {
  return 0;
}

auto list_files(std::vector<token::Token>& args) -> int {
  namespace fs = std::filesystem;

  for (auto& f : fs::directory_iterator(".")) {
    if (f.is_directory()) {
      std::cout << BLUE;
    } else {
      std::cout << GREEN;
    }
    std::cout << f.path().filename().c_str() << " ";
  }

  std::cout << CLEAR << std::endl;

  return 1;
}

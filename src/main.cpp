
#include <sstream>

// Linux specific includes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// my headers
#include "builtins.hpp"
#include "common.hpp"

// function declarations
auto cmd_loop() -> void;
auto parse_line(const std::string line) -> int;
auto launch_args(const std::vector<std::string> line) -> int;
auto execute_cmds(const std::vector<std::string> line) -> int;

auto main() -> int {

  std::cout << "\033[35mMash Version: " << VERSION << std::endl;
  std::cout << "WARNING: This is an early version, everything is subject to change" << std::endl;
  std::cout << "\033[0m" << std::endl; // reset colours

  //cmd interp loop
  cmd_loop();

  return 0;
}

// function definitions
// Basic shell loop goes read, parse, execute
auto cmd_loop() -> void {
  using namespace std;
  namespace fs = filesystem;
  int status;
  string line;

  do {
    cout << fs::current_path().c_str() << ">> ";
    getline(cin, line);
    status = parse_line(line);
  } while (status);
}

auto parse_line(const std::string line) -> int {
  auto tokens = tokenize(line);

  return execute_cmds(tokens);
}

auto execute_cmds(const std::vector<std::string> line) -> int {

  if (line.size() == 0 || line[0] == "")
    return 1;

  auto v = builtins[line[0]];
  if (v != NULL) {
    return v(line);
  }

  //return launch_args(line);
  ERROR("\"" << line[0] << "\"" << " is not a recognized command")
  return 1;
}

auto launch_args(const std::vector<std::string> line) -> int {
  pid_t pid;
  int status;

  std::vector<const char*> args;
  for (const auto& token : line)
    args.push_back(token.data());
  args.push_back(NULL); // string terminating character
  args.shrink_to_fit();

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], const_cast<char* const*>(args.data())) == -1) {
      perror("mash");
    }
    exit(-1);
  } else if (pid < 0) {
    // error
    perror("mash");
  } else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

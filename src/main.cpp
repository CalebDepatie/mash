#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <filesystem>

// Linux specific includes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define VERSION "0.0.1"

// built in shell commands
auto change_dir(const std::vector<std::string> args) -> int;
auto quit(const std::vector<std::string> args) -> int;

// todo: aliasing
const std::string builtin_commands[] = {
  "Change-Directory",
  "Quit"
};

int (*builtin_funcs[]) (const std::vector<std::string>) = {
  &change_dir,
  &quit
};

// function declarations
auto cmd_loop() -> void;
auto parse_line(const std::string line) -> int;
auto tokenize(const std::string str) -> std::vector<std::string>;
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
    string path = fs::path("/");
    cout << path << endl;
    cout << ">> ";
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

  // need to have a better method of this to allow for user defined. hashtable maybe?
  // todo: make cmd's case insensitive
  for (int i = 0; i < (sizeof(builtin_commands) / sizeof(char*)); i++)
    if (strcmp(line[0].c_str(), builtin_commands[i].c_str()) == 0)
      return (*builtin_funcs[i])(line);

  return launch_args(line);
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

auto tokenize(const std::string str) -> std::vector<std::string> {
  using namespace std;
  vector<string> tokens;
  stringstream str_stream(str);
  string arg;
  while(getline(str_stream, arg, ' '))
    tokens.emplace_back(arg);

  return tokens;
}

// built in shell commands

auto change_dir(const std::vector<std::string> args) -> int {
  if (args.size() == 1) {\
    std::cerr << "mash: expected argument to \"Change-Directory\"" << std::endl;
  } else {

  }
  return 1;
}

auto quit(const std::vector<std::string> args) -> int {
  return 0;
}

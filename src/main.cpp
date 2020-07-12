#include <sstream>

// Linux specific includes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// my headers
#include "builtins.h"
#include "lexer.h"

// function declarations
auto cmd_loop() -> void;
inline auto parse_line(const std::string line) -> int;
auto launch_args(const std::queue<std::string> line) -> int;
auto execute_cmds(const std::queue<token::Token> tokens) -> int;

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
  auto args = args_splitter(line);
  auto tokens = lexer::lex(args);

  return execute_cmds(tokens);
}

auto execute_cmds(std::queue<token::Token> tokens) -> int {

  // check if input is valid
  if (tokens.size() == 0)
    return 1;
  token::Token front_arg = pop_front(tokens);

  switch (front_arg.type) {
    using namespace token;
    case token_type::Reserved:
      auto v = builtins[front_arg.value];
      return v(tokens);
      break;
  }

  //return launch_args(line);
  ERROR("\"" << front_arg.value << "\"" << " is not a recognized command")
  return 1;
}

/* will have to redo this later
auto launch_args(const std::queue<std::string> line) -> int {
  pid_t pid;
  int status;

  std::queue<const char*> args;
  for (const auto& token : line)
    args.emplace(token.data());
  args.emplace(NULL); // string terminating character

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
} */

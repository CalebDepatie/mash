#include <sstream>

// Linux specific includes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// my headers
#include "builtins.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluation.hpp"
#include "tokens.hpp"
#include "common.hpp"

// function declarations
auto cmd_loop() -> void;
inline auto parse_line(const std::string line) -> int;
auto launch_args(const std::vector<std::string> line) -> int;
auto execute_cmds(parser::ASTNode* top) -> int;

auto main() -> int {

  if constexpr(DEBUG) {
    std::cout << PURPLE << "Mash Version: " << VERSION << std::endl;
    std::cout << "WARNING: This is an early version, everything is subject to change" << std::endl;
    std::cout << CLEAR << std::endl; // reset colours
  }

  //cmd interp loop
  cmd_loop();

  return 0;
}

// function definitions
// Basic shell loop goes read, parse, execute
// this is also functionally similar to a REPL
auto cmd_loop() -> void {
  using namespace std;
  namespace fs = filesystem;
  int status;
  string line;

  do {
    cout << fs::current_path().c_str() << LINE_SYMBOL;
    getline(cin, line);
    status = parse_line(line);
  } while (status);
}

auto parse_line(const std::string line) -> int {
  //auto args = args_splitter(line);
  auto tokens = lexer::lex(line);
  auto* ast = parser::parse(tokens);

  return execute_cmds(ast);
}

auto execute_cmds(parser::ASTNode* top) -> int {

  print_debug(top->toString());

  //clean up memory, should call all the deconstructors and delete all heap allocated mem
  delete top;

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

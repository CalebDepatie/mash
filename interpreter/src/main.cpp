#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <libsocket/exception.hpp>
#include <libsocket/unixclientstream.hpp>
#include <sstream>

#include "common.hpp"
#include "frontend/ast.hpp"
#include "frontend/lexer.hpp"
#include "frontend/parser.hpp"
#include "frontend/tokens.hpp"
#include "middleend.hpp"

// function declarations
auto cmd_loop() -> void;
auto parse_line(const std::string line) -> int;
auto parse_file(const std::string file) -> std::shared_ptr<parser::Scope>;
auto launch_args(const std::vector<std::string> line) -> int;
auto execute_cmds(std::vector<ExecKey> exec_list) -> int;

auto main(int argc, char* argv[]) -> int {
  if constexpr (DEBUG) {
    std::cout << PURPLE << "Mash Version: " << VERSION << std::endl;
    std::cout << "WARNING: This is an early version, everything is subject to change" << std::endl;
    std::cout << CLEAR << std::endl;  // reset colours
  }

  // determine if reading file or repl
  if (argc == 1) {
    // cmd interp loop
    cmd_loop();

  } else if (argc == 2) {
    // run file
    std::ifstream script(argv[1]);

    if (!script.is_open()) {
      print_error("Could not open file " + std::string(argv[1]) + ", does it exist?");

      return 1;
    }

    std::string file((std::istreambuf_iterator<char>(script)), std::istreambuf_iterator<char>());

    script.close();

    // parse whole file to AST
    auto ast = parse_file(file);

    // Bake whole file
    auto baked = bakeAST(ast);

    if constexpr (DEBUG) {
      print_debug("Baked AST:");
      print_debug(baked.DebugString());
    }

    // Send to daemon
    std::string program;
    if (!baked.SerializeToString(&program)) {
      print_error("Could not serialize baked AST to binary");
      return 1;
    }

    try {
      libsocket::unix_stream_client sock("/var/run/mash.sock");
      // I need to add a recognizable EOF for golang
      sock.snd(program.data(), program.size());
      sock.shutdown();  // indicate theres no more messages to send

      // size is a guess...
      const size_t s   = 256;
      char*        ret = new char[s];
      memset(ret, 0, s);

      sock.rcv(ret, s - 1);

      std::string return_string(ret);

      ExecResponse return_data;

      if (!return_data.ParseFromString(return_string)) {
        print_error("Could not parse returned data");
        return 1;
      }

      std::cout << "Response: " << return_data.result() << std::endl;

    } catch (const libsocket::socket_exception& exc) { print_error(exc.mesg); }

  } else {
    print_error("Expected 1 file or none");
  }

  return 0;
}

// function definitions
// Basic shell loop goes read, parse, execute
// this is also functionally similar to a REPL
auto cmd_loop() -> void {
  using namespace std;
  namespace fs = filesystem;
  int    status;
  string line;

  do {
    cout << fs::current_path().c_str() << LINE_SYMBOL;
    getline(cin, line);
    status = parse_line(line);
  } while (status);
}

auto parse_line(const std::string line) -> int {
  // auto args = args_splitter(line);
  auto tokens = lexer::lex(line);

  // middle end will handle this, empty vecs were being converted to AST :(
  if (tokens.size() == 0)
    return 0;

  int  pc  = 0;  // this is really evaluating a scope, so an initialized parse counter is required
  auto ast = parser::parse(tokens, pc);

  print_debug(ast->toString());

  auto exec_list = bakeAST(ast);

  // return execute_cmds(exec_list);
}

auto parse_file(const std::string file) -> std::shared_ptr<parser::Scope> {
  std::shared_ptr<parser::Scope> ast_top;

  auto tokens = lexer::lex(file);

  if (tokens.size() == 0)
    return ast_top;

  int pc  = 0;  // this is really evaluating a scope, so an initialized parse counter is required
  ast_top = parser::parse(tokens, pc);

  print_debug(ast_top->toString());

  return ast_top;
}

auto execute_cmds(std::vector<ExecKey> exec_list) -> int {
  // if constexpr (DEBUG) {
  //   std::cout << std::endl;
  //   for (auto& exec_key : exec_list) {
  //     print_debug(exec_key.toString());
  //   }
  //   std::cout << std::endl;
  // }

  // this will send to the daemon

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

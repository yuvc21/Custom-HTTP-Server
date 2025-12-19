#include "server.hpp"
#include <iostream>

int main() {
  // flushing after every std::cout / std::cerr;
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // you can use print statements as follows for debugging,
  // they'll be visible while running tests;
  std::cout << "Logs from the program will appear here!" << std::endl;

  // create and setup server
  Server server;
  if (!server.setup()) {
    return 1;
  }

  // accept and handle one client
  int client_fd = server.acceptClient();
  if (client_fd >= 0) {
    server.handleClient(client_fd);
  }

  return 0;
}
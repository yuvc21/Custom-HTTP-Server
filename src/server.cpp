#include "server.hpp"
#include "http_parser.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h> // send(), recv(), socket(), bind(), listen(), accept()
#include <thread>
// these calls are system calls they are part of operating system's(linux here)
// API for network programming not function of CPP and are somewhat different
// for windows
#include <unistd.h> // close(), read(), write()

/* When you call:
send(client_socket, response.c_str(), response.length(), 0);

 Your program asks the Linux kernel:
 "Please send this data through network socket #X"
*/

Server::Server(const std::string &directory)
    : server_fd(-1), filesDirectory(directory) {}

Server::~Server() {
  if (server_fd >= 0) {
    close(server_fd);
  }
}

bool Server::setup() {
  // create socket;
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "Failed to create server socket\n";
    return false;
  }

  // since the tester restarts your program quite often, setting SO_REUSEADDR;
  //  ensures that we don't run into 'Address already in use' error;
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    std::cerr << "setsocket failed\n";
    return false;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) !=
      0) {
    std::cerr << "Failed to bind port 4221\n";
    return false;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return false;
  }
  return true;
}

int Server::acceptClient() {
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  // it blocks code up till here until a client is connected;
  std::cout << "Waiting for a client to connect..." << std::endl;
  int client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                         (socklen_t *)&client_addr_len);
  std::cout << "Client Connected" << std::endl;

  return client_fd;
}

void Server::start() {
  std::cout << "Server is running on port 4221...." << std::endl;

  // infinite loop to keep accepting connections
  while (true) {
    int client_fd = acceptClient();

    if (client_fd < 0) {
      std::cerr << "Failed to accept client" << std::endl;
      continue; // skipping the current iteration, trying again
    }

    // starting new thread to handle this client concurrently!!!
    std::thread clientThread([this, client_fd] { handleClient(client_fd); });

    // detatching the thread(runs independently, doesn't block)
    clientThread.detach();

    // server immidiately ready for next client
    std::cout << "Client spawned in new thread, ready for next..." << std::endl;
  }
}
void Server::handleClient(int client_fd) {
  // reading the request
  char buffer[4096] = {0};
  read(client_fd, buffer, sizeof(buffer));

  std::string request(buffer);

  // extracting path
  std::string path = HttpParser::extractPath(request);

  // extracting the method
  std::string method = HttpParser::extractMethod(request);

  // extracting body
  std::string body = HttpParser::extractBody(request);

  std::cout << "Request: " << method << " " << path << std::endl;

  // generating responses with method support
  std::string response =
      HttpParser::generateResponse(method, path, body, filesDirectory);

  // sending response
  send(client_fd, response.c_str(), response.length(), 0);

  // closing the client connection
  close(client_fd);
}
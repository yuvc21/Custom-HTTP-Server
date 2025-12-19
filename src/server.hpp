#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>

class Server {
private:
  int server_fd;

public:
  Server();
  ~Server();

  // setup and binding server to port 4221
  bool setup();

  // accepting a client connection
  int acceptClient();

  //handling a client request and send response
  void handleClient(int client_fd);
};

#endif //SERVER_HPP
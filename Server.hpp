#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <poll.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class Server {
private:
  int server_fd;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  socklen_t client_addr_size;
  std::vector<pollfd> poll_fds;

  void setServerSocket();
  void setServerAddr();
  void setServerBind();
  void setServerListen();
  void handleClient(int client_fd);

public:
  Server(const std::string &port_num);
  ~Server();
  void run();
};

#endif // __SERVER_HPP__

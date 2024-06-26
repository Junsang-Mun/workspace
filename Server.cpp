#include "Server.hpp"
#include <algorithm> // std::remove

#define MAX_CLIENTS 256

Server::Server(const std::string &port_num) {
  setServerSocket();
  setServerAddr();
  setServerBind();
  setServerListen();
  client_addr_size = sizeof(client_addr);

  pollfd server_pollfd;
  server_pollfd.fd = server_fd;
  server_pollfd.events = POLLIN;
  poll_fds.push_back(server_pollfd);
}

Server::~Server() {
  if (server_fd != -1)
    close(server_fd);
  for (const auto &pfd : poll_fds) {
    if (pfd.fd != -1 && pfd.fd != server_fd)
      close(pfd.fd);
  }
}

void Server::setServerSocket() {
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    throw std::runtime_error("socket error");
  }

  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    throw std::runtime_error("setsockopt error");
  }
}

void Server::setServerAddr() {
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(6667); // IRC 기본 포트
}

void Server::setServerBind() {
  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    throw std::runtime_error("bind error");
  }
}

void Server::setServerListen() {
  if (listen(server_fd, 5) == -1) {
    throw std::runtime_error("listen error");
  }
}

void Server::run() {
  while (true) {
    int n = poll(poll_fds.data(), poll_fds.size(), -1);
    if (n == -1) {
      throw std::runtime_error("poll error");
    }

    for (size_t i = 0; i < poll_fds.size(); i++) {
      if (poll_fds[i].revents & POLLIN) {
        if (poll_fds[i].fd == server_fd) {
          int new_socket = accept(server_fd, (struct sockaddr *)&client_addr,
                                  &client_addr_size);
          if (new_socket == -1) {
            throw std::runtime_error("accept error");
          }

          if (poll_fds.size() >= MAX_CLIENTS) {
            close(new_socket);
            throw std::runtime_error("max clients reached");
          }

          pollfd client_pollfd;
          client_pollfd.fd = new_socket;
          client_pollfd.events = POLLIN;
          poll_fds.push_back(client_pollfd);

          std::cout << "New client connected: " << new_socket << std::endl;
        } else {
          int client_fd = poll_fds[i].fd;
          char buffer[512];
          int n = read(client_fd, buffer, sizeof(buffer) - 1);
          if (n <= 0) {
            if (n == 0) {
              // Connection closed by client
              std::cout << "Client disconnected: " << client_fd << std::endl;
            } else {
              // Read error
              std::cerr << "Read error from client: " << client_fd << std::endl;
            }
            close(client_fd);
            poll_fds.erase(poll_fds.begin() + i);
            i--; // 클라이언트를 제거했으므로 인덱스를 하나 줄여야 함
          } else {
            buffer[n] = '\0';
            std::cout << "Received from " << client_fd << ": " << buffer
                      << std::endl;

            // 클라이언트 메시지 처리 로직 추가
          }
        }
      }
    }
  }
}

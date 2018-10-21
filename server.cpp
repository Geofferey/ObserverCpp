#include "server.hpp"
#include <iostream>

using namespace std;

const void error(const char *error) {
  perror(error);
  exit(EXIT_FAILURE);
}

Server::Server() { port = 0; }

Server::Server(const char port[]) : port((char *)port) { initAddrInfo(); };

char *Server::getPort() { return port; };

const void Server::initAddrInfo() {
  struct addrinfo hints;

  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(NULL, port, &hints, &res);
};

const void Server::start() {
  initAndListen();
  createClient();

  send(clients.front().getSockfd(), "Hello", sizeof("Hello"), 0);

  destroyClient(clients.front());
  close(sockfd);
};

const void Server::createClient() {
  Client c;

  int clientfd;

  if ((clientfd = accept(sockfd, (struct sockaddr *)c.getClientAddr(),
                         c.getClientAddrLen())) < 0) {
    error("accept failed");
  }

  c = Client(clientfd);

  if (c.getProcessId() == 0) {
    cout << "Welcome to child process";
  }

  clients.push_front(c);
}

const void Server::destroyClient(Client &c) { c.end(); }

const void Server::initAndListen() {
  if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <
      0) {
    error("Problem in creating the listening socket");
  }

  setReusable(1);

  if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
    error("bind failed");
  }

  freeaddrinfo(res);

  if (listen(sockfd, 3) < 0) {
    error("listen failed");
  }
};

const void Server::setReusable(int reuse = 1) {
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                 (const char *)&reuse, sizeof(reuse)) < 0) {
    error("setsockopt failed");
  }
}
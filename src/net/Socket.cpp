/**
 * @file Socket.cpp
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 套接字描述符wrapper实现
 * @version 0.1
 * @date 2024-07-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "net/Socket.h"
#include "Retval.h"
#include "net/NetAddress.h"
#include "net/SocketOps.h"
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
using namespace neonet;
Socket::Socket(int sockfd) : m_sockfd(sockfd) {}

Socket::~Socket() {
  if (m_sockfd != -1) {
    close(m_sockfd);
  }
}

int Socket::setReuseAddr() {

  int opt = 1;
  if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    strerror(errno);
    throw std::logic_error("Socket: setReuseAddr() Error");
  }
  return SUCCESS;
}

int Socket::setReusePort() {
  int opt = 1;
  if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    strerror(errno);
    throw std::logic_error("Socket: setReusePort() Error");
  }

  return SUCCESS;
}

int Socket::setNonblock() {
  int flags = fcntl(m_sockfd, F_GETFL, 0);
  if (flags < 0) {
    strerror(errno);
    throw std::logic_error("Socket: setNonblock() Error");
  }
  if (fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
    strerror(errno);
    throw std::logic_error("Socket: setNonblock() Error");
  }
  return SUCCESS;
}

int Socket::setNoDelay() {
  int opt = 1;
  if (setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0) {
    strerror(errno);
    throw std::logic_error("Socket: setNoDelay() Error");
  }
  return SUCCESS;
}

int Socket::setKeepAlive() {
  int opt = 1;
  if (setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
    strerror(errno);
    throw std::logic_error("Socket: setKeepAlive() Error");
  }
  return SUCCESS;
}

int Socket::bind(const NetAddress &addr, bool reuse) {
  if (reuse) {
    setReuseAddr();
    setReusePort();
  }
  socket::bind(m_sockfd, socket::sockaddr_cast(&(addr.getAddr())));
  return SUCCESS;
}

int Socket::listen(int backlog) {
  socket::listen(backlog);
  return SUCCESS;
}

int Socket::accept(NetAddress *peeraddr) {
  struct sockaddr_in addr;
  int connfd = socket::accept(m_sockfd, &addr);
  if (connfd >= 0) {
    peeraddr->setAddr(addr);
  }
  return connfd;
}

int Socket::connect(const NetAddress &addr) {
  if (::connect(m_sockfd, (struct sockaddr *)&(addr.getAddr()),
                addr.getAddrLen()) < 0) {
    strerror(errno);
    throw std::logic_error("Socket: connect() Error");
  }
  return SUCCESS;
}

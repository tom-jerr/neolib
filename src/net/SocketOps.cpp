/**
 * @file SocketOps.cpp
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 对socket的操作进行封装
 * @version 0.1
 * @date 2024-07-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "net/SocketOps.h"
#include "Config.h"
#include "tools/Bytetransform.h"
#include "tools/memtools.h"
#include <cassert>
#include <cstdint>
#include <cstdio> // snprintf
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sys/uio.h> // readv
#include <unistd.h>
using namespace neonet;

const struct sockaddr *socket::sockaddr_cast(const struct sockaddr_in *addr) {
  return static_cast<const struct sockaddr *>(
      reinterpret_cast<const void *>(addr));
}

const struct sockaddr_in *
socket::sockaddr_in_cast(const struct sockaddr *addr) {
  return static_cast<const struct sockaddr_in *>(
      reinterpret_cast<const void *>(addr));
}

int socket::createNonblocking() {
  int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                        IPPROTO_TCP);
  if (sockfd < 0) {
    std::cout << "sockets::createNonblockingOrDie";
  }
  return sockfd;
}

void socket::bind(int sockfd, const struct sockaddr *addr) {
  int ret =
      ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
  if (ret < 0) {
    std::cout << "sockets::bindOrDie";
  }
}

void socket::listen(int sockfd) {
  int ret = ::listen(sockfd, BACKLOG);
  if (ret < 0) {
    std::cout << "sockets::listenOrDie";
  }
}

int socket::accept(int sockfd, struct sockaddr_in *addr) {
  socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
  // accept4是一个linux特有的系统调用，可以在调用accept的同时设置socket的属性，这里设置为非阻塞和关闭时释放文件描述符
  int connfd =
      ::accept4(sockfd, const_cast<struct sockaddr *>(sockaddr_cast(addr)),
                &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);

  if (connfd < 0) {
    int savedErrno = errno;
    std::cout << "Socket::accept";
    switch (savedErrno) {
    case EAGAIN:
    case ECONNABORTED:
    case EINTR:
    case EPROTO: // ???
    case EPERM:
    case EMFILE: // per-process lmit of open file desctiptor ???
      // expected errors
      errno = savedErrno;
      break;
    case EBADF:
    case EFAULT:
    case EINVAL:
    case ENFILE:
    case ENOBUFS:
    case ENOMEM:
    case ENOTSOCK:
    case EOPNOTSUPP:
      // unexpected errors
      std::cout << "unexpected error of ::accept " << savedErrno;
      break;
    default:
      std::cout << "unknown error of ::accept " << savedErrno;
      break;
    }
  }
  return connfd;
}

int socket::connect(int sockfd, const struct sockaddr *addr) {
  return ::connect(sockfd, addr,
                   static_cast<socklen_t>(sizeof(struct sockaddr_in)));
}

ssize_t socket::read(int sockfd, void *buf, size_t count) {
  return ::read(sockfd, buf, count);
}

ssize_t socket::readv(int sockfd, const struct iovec *iov, int iovcnt) {
  return ::readv(sockfd, iov, iovcnt);
}

ssize_t socket::write(int sockfd, const void *buf, size_t count) {
  return ::write(sockfd, buf, count);
}

void socket::close(int sockfd) {
  if (::close(sockfd) < 0) {
    std::cout << "sockets::close";
  }
}

void socket::shutdownWrite(int sockfd) {
  if (::shutdown(sockfd, SHUT_WR) < 0) {
    std::cout << "sockets::shutdownWrite";
  }
}

void socket::toIpPort(char *buf, size_t size, const struct sockaddr *addr) {
  toIp(buf, size, addr);
  size_t end = ::strlen(buf);
  const struct sockaddr_in *addr4 = sockaddr_in_cast(addr);
  uint16_t port = socket::networkToHost16(addr4->sin_port);
  assert(size > end);
  snprintf(buf + end, size - end, ":%u", port);
}

void socket::toIp(char *buf, size_t size, const struct sockaddr *addr) {
  if (addr->sa_family == AF_INET) {
    assert(size >= INET_ADDRSTRLEN);
    const struct sockaddr_in *addr4 = sockaddr_in_cast(addr);
    ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
  }
}

void socket::fromIpPort(const char *ip, uint16_t port,
                        struct sockaddr_in *addr) {
  addr->sin_family = AF_INET;
  addr->sin_port = hostToNetwork16(port);
  if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
    std::cout << "sockets::fromIpPort";
  }
}

int socket::getSocketError(int sockfd) {
  int optval;
  socklen_t optlen = static_cast<socklen_t>(sizeof optval);

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
    return errno;
  } else {
    return optval;
  }
}

struct sockaddr socket::getLocalAddr(int sockfd) {
  struct sockaddr localaddr;
  memZero(&localaddr, sizeof localaddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
  if (::getsockname(sockfd, &localaddr, &addrlen) < 0) {
    std::cout << "sockets::getLocalAddr";
  }
  return localaddr;
}

struct sockaddr socket::getPeerAddr(int sockfd) {
  struct sockaddr peeraddr;
  memZero(&peeraddr, sizeof peeraddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
  if (::getpeername(sockfd, &peeraddr, &addrlen) < 0) {
    std::cout << "sockets::getPeerAddr";
  }
  return peeraddr;
}

bool socket::isSelfConnect(int sockfd) {
  struct sockaddr localaddr = getLocalAddr(sockfd);
  struct sockaddr peeraddr = getPeerAddr(sockfd);
  if (localaddr.sa_family == AF_INET) {
    const struct sockaddr_in *laddr4 =
        reinterpret_cast<struct sockaddr_in *>(&localaddr);
    const struct sockaddr_in *raddr4 =
        reinterpret_cast<struct sockaddr_in *>(&peeraddr);
    return laddr4->sin_port == raddr4->sin_port &&
           laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
  } else {
    return false;
  }
}

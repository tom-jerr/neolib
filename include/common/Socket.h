/**
 * @file Socket.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 原生TCP套接字封装
 * @version 0.1
 * @date 2024-07-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef SOCKET_H
#define SOCKET_H
#include "Config.h"
#include <unistd.h>

class NetAddress;

class Socket {
public:
  Socket() = default;
  explicit Socket(int sockfd);
  ~Socket();

  // nocopy
  Socket(const Socket &sock) = delete;
  Socket &operator=(const Socket &sock) = delete;

public:
  int getFd() const { return m_sockfd; }
  void setFd(int sockfd) { m_sockfd = sockfd; }
  /**
   * @brief 对套接字进行绑定地址
   *
   * @param addr
   * @param reuse
   * @return int
   */
  int bind(const NetAddress &addr, bool reuse = true);
  /**
   * @brief 监听套接字，设置半连接队列最大长度
   *
   * @param backlog
   * @return int
   */
  int listen(int backlog = BACKLOG);
  /**
   * @brief 接受客户端的连接
   *
   * @param addr
   * @return int
   */
  int accept(NetAddress &addr);
  /**
   * @brief 客户端发起连接
   *
   * @param addr
   * @return int
   */
  int connect(const NetAddress &addr);

private:
  /**
   * @brief Set the Reuse Addr object，解决time_wait问题
   *
   * @return int
   */
  int setReuseAddr();
  /**
   * @brief Set the Reuse Port object，解决惊群问题
   *
   * @return int
   */
  int setReusePort();
  int setNonblock();
  /**
   * @brief
   * 关闭Nagle算法，允许小包的发送。
   *
   * @return int
   */
  int setNoDelay();
  int setKeepAlive();

private:
  int m_sockfd{-1}; // 套接字描述符
};
#endif // SOCKET_H
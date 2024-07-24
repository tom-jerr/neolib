/**
 * @file Acceptor.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 负责接收客户端发来的连接请求
 * @version 0.1
 * @date 2024-07-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_

#include "net/Channel.h"
#include "net/Socket.h"
#include <functional>
namespace neonet {

class EventLoop;
class NetAddress;

class Acceptor {

public:
  using NewConnectionCallback =
      std::function<void(int sockfd, const NetAddress &)>;
  Acceptor(EventLoop *loop, const NetAddress &listenAddr);
  ~Acceptor();

  void setNewConnectionCallback(const NewConnectionCallback &cb) {
    m_newConnectionCallback = cb;
  }

  bool listenning() const { return m_listenning; }
  void listen();

private:
  EventLoop *m_loop;                             // 所属EventLoop
  Socket m_acceptSocket;                         //  接受套接字
  Channel m_acceptChannel;                       // 事件分发器
  NewConnectionCallback m_newConnectionCallback; // 新连接回调函数
  bool m_listenning{false};                      // 是否监听
  int m_idleFd; // 空闲fd，用于处理EMFILE
};

} // namespace neonet

#endif // ACCEPTOR_H_
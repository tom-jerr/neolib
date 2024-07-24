/**
 * @file Acceptor.cpp
 * @author lzy (lzy_cs_LN@163.com)
 * @brief
 * @version 0.1
 * @date 2024-07-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "net/Acceptor.h"
#include "net/EventLoop.h"
#include "net/NetAddress.h"
#include "net/Socket.h"
#include "net/SocketOps.h"
#include <cassert>
#include <fcntl.h>
#include <iostream>
using namespace neonet;

Acceptor::Acceptor(EventLoop *loop, const NetAddress &listenAddr)
    : m_loop(loop), m_acceptSocket(socket::createNonblocking()),
      m_acceptChannel(loop, m_acceptSocket.fd()), m_listenning(false),
      m_idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
  assert(m_idleFd >= 0);
  m_acceptSocket.bind(listenAddr);

  /**
   * @brief acceptchannel的回调函数
   *
   */
  auto acceptconn = [this]() {
    m_loop->assertInLoopThread();
    NetAddress peerAddr;
    int connfd = m_acceptSocket.accept(&peerAddr);
    if (connfd >= 0) {
      if (m_newConnectionCallback) {
        m_newConnectionCallback(connfd, peerAddr);
      } else {
        socket::close(connfd);
      }
    } else {
      std::cout << "Acceptor::acceptconn error";
      if (errno == EMFILE) {
        ::close(m_idleFd);
        m_idleFd = ::accept(m_acceptSocket.fd(), nullptr, nullptr);
        ::close(m_idleFd);
        m_idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
      }
    }
  };
  m_acceptChannel.setReadCallback(acceptconn);
}

Acceptor::~Acceptor() {
  m_acceptChannel.disableAll();
  m_acceptChannel.remove();
  ::close(m_idleFd);
}

void Acceptor::listen() {
  m_loop->assertInLoopThread();
  m_listenning = true;
  m_acceptSocket.listen();
  m_acceptChannel.enableReading();
}
/**
 * @file TCPConnection.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 建立并维护两个对象之间的TCP连接
 * @version 0.1
 * @date 2024-07-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_
#include "base/Callbacks.h"
#include "net/Buffer.h"
#include "net/NetAddress.h"
#include <cstddef>
#include <memory>
#include <string>
namespace neonet {
class Channel;
class EventLoop;
class Socket;

class TCPConnection : public std::enable_shared_from_this<TcpConnection> {
public:
  TCPConnection(EventLoop *loop, const std::string &name, int sockfd,
                const NetAddress &localAddr, const NetAddress &peerAddr);
  ~TCPConnection();

  // noncopy
  TCPConnection(const TCPConnection &) = delete;
  TCPConnection &operator=(const TCPConnection &) = delete;

  EventLoop *loop() { return m_loop; }
  const std::string &name() { return m_name; }
  const NetAddress &localAddress() { return m_localAddr; }
  const NetAddress &peerAddress() { return m_peerAddr; }
  bool connected() const { return m_state == kConnected; }
  bool disconnected() const { return m_state == kDisconnected; }

  void send(const void *message, int len);
  void send(Buffer *message);
  void shutdown();
  void forceClose();
  void setTcpNoDelay(bool on);
  void startRead();
  void stopRead();
  bool isReading() const { return m_reading; }

  void setConnectionCallback(const ConnectionCallback &cb) {
    m_connectionCallback = cb;
  }

  void setMessageCallback(const MessageCallback &cb) { m_messageCallback = cb; }

  void setWriteCompleteCallback(const WriteCompleteCallback &cb) {
    m_writeCompleteCallback = cb;
  }

  void setHighWaterMarkCallback(const HighWaterMarkCallback &cb,
                                size_t highWaterMark) {
    m_highWaterMarkCallback = cb;
    m_highWaterMark = highWaterMark;
  }

  /// Advanced interface
  Buffer *inputBuffer() { return &m_inputBuffer; }

  Buffer *outputBuffer() { return &m_outputBuffer; }

  /// Internal use only.
  void setCloseCallback(const CloseCallback &cb) { m_closeCallback = cb; }

  // called when TcpServer accepts a new connection
  void connectEstablished(); // should be called only once
  // called when TcpServer has removed me from its map
  void connectDestroyed(); // should be called only once

private:
  enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };

  void sendInLoop(const void *message, size_t len);
  void shutdownInLoop();
  // void shutdownAndForceCloseInLoop(double seconds);
  void forceCloseInLoop();
  void setState(StateE s) { m_state = s; }
  const char *stateToString() const;
  void startReadInLoop();
  void stopReadInLoop();

  EventLoop *m_loop;
  const std::string m_name;
  StateE m_state; // FIXME: use atomic variable
  bool m_reading;
  // we don't expose those classes to client.
  std::unique_ptr<Socket> m_socket;
  std::unique_ptr<Channel> m_channel;
  const NetAddress m_localAddr;
  const NetAddress m_peerAddr;
  ConnectionCallback m_connectionCallback;
  MessageCallback m_messageCallback;
  WriteCompleteCallback m_writeCompleteCallback;
  HighWaterMarkCallback m_highWaterMarkCallback;
  CloseCallback m_closeCallback;
  size_t m_highWaterMark;
  Buffer m_inputBuffer;
  Buffer m_outputBuffer; // FIXME: use list<Buffer> as output buffer.
};
using TCPConnectionPtr = std::shared_ptr<TCPConnection>;
} // namespace neonet
#endif // TCPCONNECTION_H_
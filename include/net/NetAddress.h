/**
 * @file NetAddress.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 服务器、客户端地址类
 * @version 0.1
 * @date 2024-07-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef NETADDRESS_H
#define NETADDRESS_H
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
namespace neonet {
class NetAddress {
public:
  NetAddress();
  explicit NetAddress(const char *ip, int port);
  explicit NetAddress(const sockaddr_in &addr);
  ~NetAddress() = default;

  // nocopy
  NetAddress(const NetAddress &addr) = delete;
  NetAddress &operator=(const NetAddress &addr) = delete;

public:
  const struct sockaddr_in &getAddr() const { return m_addr; }
  const socklen_t getAddrLen() const { return m_addr_len; }
  socklen_t *getAddrLenPtr() { return &m_addr_len; }
  const char *getIp() const { return inet_ntoa(m_addr.sin_addr); }
  int getPort() const { return ntohs(m_addr.sin_port); }
  void setAddr(const struct sockaddr_in &addr) { m_addr = addr; }

private:
  struct sockaddr_in m_addr;
  socklen_t m_addr_len;
};
} // namespace neonet
#endif // NETADDRESS_H
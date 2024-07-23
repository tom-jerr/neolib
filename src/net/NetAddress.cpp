/**
 * @file NetAddress.cpp
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 服务器、客户端地址类
 * @version 0.1
 * @date 2024-07-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "net/NetAddress.h"
#include <cstring>
using namespace neonet;
NetAddress::NetAddress() : m_addr_len(sizeof(m_addr)) {
  memset(&m_addr, 0, m_addr_len);
}

NetAddress::NetAddress(const char *ip, int port) : m_addr_len(sizeof(m_addr)) {
  memset(&m_addr, 0, m_addr_len);
  m_addr.sin_family = AF_INET;
  m_addr.sin_port = htons(port);
  m_addr.sin_addr.s_addr = inet_addr(ip);
}

NetAddress::NetAddress(const sockaddr_in &addr)
    : m_addr(addr), m_addr_len(sizeof(m_addr)) {}

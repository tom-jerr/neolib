/**
 * @file Request.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief
 * @version 0.1
 * @date 2024-07-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <cstdint>
#ifndef REQUEST_H
#define REQUEST_H
namespace neonet {
struct Header {
  uint32_t cmd;    // 消息类型，1表示请求，2表示响应
  uint32_t length; // 请求报文体长度

  Header() : cmd(0), length(0) {}
} __attribute__((packed));
} // namespace neonet
#endif // REQUEST_H
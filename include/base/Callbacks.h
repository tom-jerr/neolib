/**
 * @file Callbacks.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief
 * @version 0.1
 * @date 2024-07-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef CALLBACKS_H_
#define CALLBACKS_H_
#include <functional>
#include <memory>
namespace neonet {
class Buffer;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using TimerCallback = std::function<void()>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
using HighWaterMarkCallback =
    std::function<void(const TcpConnectionPtr &, size_t)>;

// the data has been read to (buf, len)
using MessageCallback = std::function<void(const TcpConnectionPtr &, Buffer *)>;

void defaultConnectionCallback(const TcpConnectionPtr &conn);
void defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buffer);
};     // namespace neonet
#endif // CALLBACKS_H_
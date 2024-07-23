/**
 * @file SocketOps.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 套接字操作函数
 * @version 0.1
 * @date 2024-07-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef SOCKETOPS_H
#define SOCKETOPS_H

#include <arpa/inet.h>

namespace neonet::socket {
/**
 * @brief Create a Nonblocking object
 *
 * @return int
 */
int createNonblocking();

int connect(int sockfd, const struct sockaddr *addr);
void bind(int sockfd, const struct sockaddr *addr);
void listen(int sockfd);
int accept(int sockfd, struct sockaddr_in *addr);
ssize_t read(int sockfd, void *buf, size_t count);
ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
ssize_t write(int sockfd, const void *buf, size_t count);
void close(int sockfd);
void shutdownWrite(int sockfd);

/**
 * @brief 从sockaddr中获取ip和port
 *
 * @param buf
 * @param size
 * @param addr
 */
void toIpPort(char *buf, size_t size, const struct sockaddr *addr);
void toIp(char *buf, size_t size, const struct sockaddr *addr);
/**
 * @brief 从ip和port中构造sockaddr
 *
 * @param ip
 * @param port
 * @param addr
 */
void fromIpPort(const char *ip, uint16_t port, struct sockaddr_in *addr);

int getSocketError(int sockfd);

/**
 * @brief
 * socketaddr和socketaddr_in地址转换函数；均先强转为void*后，进行static_cast转换
 *
 * @param addr
 * @return const struct sockaddr*
 */
const struct sockaddr *sockaddr_cast(const struct sockaddr_in *addr);
const struct sockaddr_in *sockaddr_in_cast(const struct sockaddr *addr);

/**
 * @brief Get the Local Addr and Peer Addr object
 *
 * @param sockfd
 * @return struct sockaddr
 */
struct sockaddr getLocalAddr(int sockfd);
struct sockaddr getPeerAddr(int sockfd);

/**
 * @brief 判断是否是自连接
 *
 * @param sockfd
 * @return true
 * @return false
 */
bool isSelfConnect(int sockfd);

} // namespace neonet::socket

#endif // SOCKETOPS_H
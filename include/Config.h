/**
 * @file Config.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 服务器和压力发生器的部分参数配置
 * @version 0.1
 * @date 2024-07-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <sys/socket.h>
struct Header;

#define PORT 12345 // 服务器端口

#define MAXEVENTS 10010 // EPOLL最大事件数，use in epoll_create
#define MAXHANDLERS 10010 // 最大的事件处理器数, use in epoll_event[MAXHANDLERS]
#define MAXCONN MAXHANDLERS       // 最大连接数
#define MAXCHARS 30001            // 最大字符数 + 1
#define BACKLOG SOMAXCONN         // listen队列的最大长度
#define GetDstId(x) (x ^ 0x1)     // 获取目的客户端编号
#define HEADERSZ (sizeof(Header)) // 头部大小
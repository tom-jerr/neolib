/**
 * @file EPoller.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 对Epoll的封装
 * @version 0.1
 * @date 2024-07-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef EPOLLER_H_
#define EPOLLER_H_

#include <map>
#include <sys/epoll.h>
#include <vector>
namespace neonet {
class Channel;
class EventLoop;

class EPoller {
public:
  using ChannelList = std::vector<Channel *>;
  EPoller(EventLoop *loop);
  ~EPoller();
  // noncopy
  EPoller(const EPoller &) = delete;
  EPoller &operator=(const EPoller &) = delete;

  /**
   * @brief 从epoll_wait返回的事件中填充活跃的Channel
   *
   * @param activeChannels
   */
  void epoll(ChannelList *activeChannels);
  /**
   * @brief 更新Channel中的事件，并改变channel的索引
   *
   * @param channel
   */
  void updateChannel(Channel *channel);
  /**
   * @brief 移除Channel，只要无关注的事件就可以删除
   *
   * @param channel
   */
  void removeChannel(Channel *channel);
  /**
   * @brief 是否拥有Channel
   *
   * @param channel
   */
  bool hasChannel(Channel *channel) const;
  /**
   * @brief 判断是否是创建EventLoop的线程
   *
   */
  void assertInLoopThread() const;

private:
  static const char *operationToString(int op);
  /**
   * @brief 将epoll_wait返回的事件填充到activeChannels中
   *
   * @param numEvents
   * @param activeChannels
   */
  void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
  /**
   * @brief epoll_ctl的封装
   *
   * @param operation
   * @param channel
   */
  void update(int operation, Channel *channel);

private:
  using EventList = std::vector<struct epoll_event>;
  using ChannelMap = std::map<int, Channel *>;

  inline static const constexpr int kInitEventListSize{
      16};                         // epoll_wait最多返回的事件数
  ChannelMap m_channels;           // fd到Channel*的映射
  EventLoop *m_ownerLoop{nullptr}; // 所属EventLoop
  int m_epollFd{0};                // epoll_create1返回的fd
  EventList m_events; // 初始化为16个epoll_event，如果不够，会自动扩容
};
} // namespace neonet

#endif // EPOLLER_H_
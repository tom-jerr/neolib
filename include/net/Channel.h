/**
 * @file Channel.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 该类不拥有Channel，只是对fd的包装，TCPConnection拥有Channel
 * @version 0.1
 * @date 2024-07-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef CHANNEL_H
#define CHANNEL_H
#include <functional>
#include <memory>
#include <sys/epoll.h>
namespace neonet {

class EventLoop;

class Channel {
public:
  using EventCallback = std::function<void()>;
  Channel(EventLoop *loop, int fd);
  ~Channel();

  // noncopy
  Channel(const Channel &) = delete;
  Channel &operator=(const Channel &) = delete;

public:
  /**
   * @brief
   * 处理事件入口函数，真正处理在handleEventWithGuard；判断TCPConnection已经被释放
   *
   */
  void handleEvent();
  /**
   * @brief 设置事件回调函数
   *
   * @param cb
   */
  void setReadCallback(const EventCallback &cb) { m_readCallback = cb; }
  void setWriteCallback(const EventCallback &cb) { m_writeCallback = cb; }
  void setCloseCallback(const EventCallback &cb) { m_closeCallback = cb; }
  void setErrorCallback(const EventCallback &cb) { m_errorCallback = cb; }

  int fd() const { return m_fd; }
  int index() const { return m_index; }
  int events() const { return m_events; }
  void set_revents(int revt) { m_revents = revt; }
  bool isNoneEvent() const { return m_events == kNoneEvent; }
  EventLoop *ownerLoop() { return m_loop; }
  /**
   * @brief 开启或关闭事件，注意要更新到epoll中
   *
   */
  void enableReading() {
    m_events |= kReadEvent;
    update();
  }
  void disableReading() {
    m_events &= ~kReadEvent;
    update();
  }
  void enableWriting() {
    m_events |= kWriteEvent;
    update();
  }
  void disableWriting() {
    m_events &= ~kWriteEvent;
    update();
  }
  void disableAll() {
    m_events = kNoneEvent;
    update();
  }
  void set_index(int idx) { m_index = idx; }
  /**
   * @brief 判断是否监听了读写事件
   *
   * @return true
   * @return false
   */
  bool isWriting() const { return m_events & kWriteEvent; }
  bool isReading() const { return m_events & kReadEvent; }
  /**
   * @brief debug查看事件，真正实现在eventsToString
   *
   * @return std::string
   */
  std::string reventsToString() const;
  std::string eventsToString() const;
  /**
   * @brief 移除Channel，在eventloop中移除，实际是epoll中移除
   *
   */
  void remove();
  /**
   * @brief 将Channel和TCPConnection绑定，用于判断Channel是否已经被释放
   *
   * @param obj
   */
  void tie(const std::shared_ptr<void> &obj);

private:
  /**
   * @brief
   * 更新函数，调用eventloop的updateChannel，eventloop会调用epoll封装的update
   *
   */
  void update();
  /**
   * @brief eventsTostring真正的实现
   *
   * @param fd
   * @param ev
   * @return std::string
   */
  static std::string eventsToString(int fd, int ev);
  /**
   * @brief 根据revents中监听的事件调用相应的回调函数
   *
   */
  void handleEventWithGuard();

private:
  /**
   * @brief 事件类型
   *
   */
  inline static constexpr const int kNoneEvent{0};
  inline static constexpr const int kReadEvent{EPOLLIN | EPOLLPRI};
  inline static constexpr const int kWriteEvent{EPOLLOUT};

  EventLoop *m_loop; // 所属的事件循环
  const int m_fd;    // Channel操作的fd
  int m_index{-1};   // epoll_ctl操作状态机变化，kNew, kAdded, kDeleted
  int m_events{0};   // 关注的事件
  int m_revents{0};  // 实际发生的事件
  std::weak_ptr<void> m_tie; // 用于判断Channel是否已经被释放
  /**
   * @brief 判断标志
   *
   */
  bool m_isTied{false};        // 是否被hold住
  bool m_eventHandling{false}; // 是否处于事件处理过程中
  bool m_addedToLoop{false};   // 是否已经添加到loop中
  /**
   * @brief 事件回调函数
   *
   */
  EventCallback m_readCallback;  // 读事件回调
  EventCallback m_writeCallback; // 写事件回调
  EventCallback m_closeCallback; // 关闭事件回调
  EventCallback m_errorCallback; // 错误事件回调
};
} // namespace neonet

#endif // CHANNEL_H
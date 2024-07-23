/**
 * @file EventLoop.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief epoll的事件循环类，真正的事件添加和执行位于epoll中
 * @version 0.1
 * @date 2024-07-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
namespace neonet {
class EPoller;
class Channel;
class EventLoop {
public:
  using Functor = std::function<void()>;
  EventLoop();
  ~EventLoop();

  // noncopy
  EventLoop(const EventLoop &) = delete;
  EventLoop &operator=(const EventLoop &) = delete;

  /**
   * @brief EventLoop的主循环
   *
   * @details
   * 该函数会一直循环，直到quit函数被调用；它的工作就是不断地从epoll中获取active
   * channels，并处理每个channel上的事件
   *
   */
  void loop();
  /**
   * @brief 退出事件循环
   *
   * @details 为避免loop函数阻塞在epoll_wait中，需要执行唤醒函数
   *
   */
  void quit();
  /**
   * @brief 某个线程向该EventLoop添加任务
   *
   *  @details
   * 如果提交任务的线程就是创建EventLoop的线程，那么直接执行任务；否则，将任务添加到任务队列中
   * @param cb
   */
  void runInLoop(Functor cb);
  /**
   * @brief 将任务添加到任务队列中，唤醒EventLoop所在线程
   *
   * @param cb
   */
  void queueInLoop(Functor cb);
  /**
   * @brief 任务队列的大小
   *
   * @return size_t
   */
  size_t queueSize() const;

  /**
   * @brief 唤醒阻塞的EventLoop，向m_wakeupFd写入一个字节
   *
   */
  void wakeup();
  void updateChannel(Channel *channel);
  void removeChannel(Channel *channel);
  bool hasChannel(Channel *channel);

  /**
   * @brief 当前线程是否是创建EventLoop的线程
   *
   * @return true
   * @return false
   */
  bool isInLoopThread() const {
    return m_threadId == std::this_thread::get_id();
  }
  void assertInLoopThread() {
    if (!isInLoopThread()) {
      abortNotInLoopThread();
    }
  }

  std::thread::id threadId() const { return m_threadId; }
  bool callingPendingFunctors() const { return m_callingPendingFunctors; }
  bool eventHandling() const { return m_eventHandling; }

private:
  /**
   * @brief 如果当前线程不是创建EventLoop的线程，终止程序
   *
   */
  void abortNotInLoopThread();
  /**
   * @brief 处理任务队列中的任务
   *
   */
  void doPendingFunctors();
  /**
   * @brief 打印活跃的channel，用于调试
   *
   */
  void printActiveChannels() const; // DEBUG

  using ChannelList = std::vector<Channel *>;

private:
  bool m_looping{false}; /* atomic */
  std::atomic<bool> m_quit{false};
  bool m_eventHandling{false};          /* atomic */
  bool m_callingPendingFunctors{false}; /* atomic */
  std::thread::id m_threadId{std::this_thread::get_id()};

  std::unique_ptr<EPoller> m_epoller;
  // std::unique_ptr<TimerQueue> timerQueue_;
  int m_wakeupFd; // 用于唤醒阻塞的eventloop
  // unlike in TimerQueue, which is an internal class,
  // we don't expose Channel to client.
  std::unique_ptr<Channel> m_wakeupChannel;

  // scratch variables
  ChannelList m_activeChannels;
  Channel *m_currentActiveChannel{nullptr};

  mutable std::mutex m_mutex;
  std::vector<Functor> m_pendingFunctors; // @BuardedBy mutex_;
};

} // namespace neonet

#endif // EVENTLOOP_H_
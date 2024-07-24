/**
 * @file EventLoop.cpp
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 事件循环的实现
 * @version 0.1
 * @date 2024-07-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "net/EventLoop.h"
#include "net/Channel.h"
#include "net/EPoller.h"
#include "net/SocketOps.h"
#include <cassert>
#include <iostream>
#include <signal.h>
#include <sys/eventfd.h>
#include <sys/types.h>
using namespace neonet;
namespace {
/**
 * @brief 线程局部变量，每个线程一个EventLoop
 *
 */
__thread EventLoop *t_loopInThisThread = 0;

/**
 * @brief Create a Eventfd object, used to wakeup epoll_wait
 *
 * @return int
 */
int createEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    std::cout << "Failed in eventfd";
  }
  return evtfd;
}

/**
 * @brief 忽略SIGPIPE信号，防止出现对已经关闭的socket进行写操作导致程序退出
 *
 */
class IgnoreSigPipe {
public:
  IgnoreSigPipe() {
    ::signal(SIGPIPE, SIG_IGN);
    // LOG_TRACE << "Ignore
  };
};
} // namespace

EventLoop::EventLoop()
    : m_epoller(new EPoller(this)), m_wakeupFd(createEventfd()),
      m_wakeupChannel(new Channel(this, m_wakeupFd)) {
  std::cout << "EventLoop created " << this << " in thread " << m_threadId
            << std::endl;
  if (t_loopInThisThread) {
    std::cout << "Another EventLoop " << t_loopInThisThread
              << " exists in this thread " << m_threadId << std::endl;
  } else {
    t_loopInThisThread = this;
  }

  /**
   * @brief wakeupfd的读事件回调函数，就是读取一个8字节的数字，唤醒epoll_wait
   *
   */
  auto wakeupEpoll = [this]() {
    uint64_t one = 1;
    ssize_t n = socket::read(m_wakeupFd, &one, sizeof one);
    if (n != sizeof one) {
      std::cout << "EventLoop::handleRead() reads " << n
                << " bytes instead of 8";
    }
  };
  m_wakeupChannel->setReadCallback(wakeupEpoll);
  // 每次唤醒都会调用handleRead
  m_wakeupChannel->enableReading();
}

EventLoop::~EventLoop() {
  std::cout << "EventLoop " << this << " of thread " << m_threadId
            << " destructs in thread " << std::this_thread::get_id()
            << std::endl;
  m_wakeupChannel->disableAll();
  m_wakeupChannel->remove();
  ::close(m_wakeupFd);
  t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
  assert(!m_looping);
  assertInLoopThread();
  m_looping = true;
  m_quit = false;

  while (!m_quit) {
    m_activeChannels.clear();
    // 从epoll中获取活跃的channel
    m_epoller->epoll(&m_activeChannels);
    m_eventHandling = true;
    for (Channel *channel : m_activeChannels) {
      m_currentActiveChannel = channel;
      channel->handleEvent();
    }
    m_currentActiveChannel = nullptr;
    m_eventHandling = false;
    doPendingFunctors();
  }

  std::cout << "EventLoop " << this << " stop looping" << std::endl;
  m_looping = false;
}

void EventLoop::quit() {
  m_quit = true;
  if (!isInLoopThread()) {
    wakeup();
  }
}

void EventLoop::runInLoop(Functor cb) {
  if (isInLoopThread()) {
    cb();
  } else {
    queueInLoop(std::move(cb));
  }
}

void EventLoop::queueInLoop(Functor cb) {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pendingFunctors.push_back(std::move(cb));
  }

  if (!isInLoopThread() || m_callingPendingFunctors) {
    wakeup();
  }
}

size_t EventLoop::queueSize() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_pendingFunctors.size();
}

void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = socket::write(m_wakeupFd, &one, sizeof one);
  if (n != sizeof one) {
    std::cout << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
  }
}

void EventLoop::updateChannel(Channel *channel) {
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  m_epoller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  m_epoller->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  return m_epoller->hasChannel(channel);
}

void EventLoop::abortNotInLoopThread() {
  std::cout << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << m_threadId
            << ", current thread id = " << std::this_thread::get_id()
            << std::endl;
}

void EventLoop::doPendingFunctors() {
  std::vector<Functor> functors;
  m_callingPendingFunctors = true;
  // 交换pendingFunctors和functors，减小临界区
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    functors.swap(m_pendingFunctors);
  }

  for (const Functor &functor : functors) {
    functor();
  }
  m_callingPendingFunctors = false;
}

void EventLoop::printActiveChannels() const {
  for (const Channel *channel : m_activeChannels) {
    std::cout << "{" << channel->reventsToString() << "} ";
  }
}
/**
 * @file EPoller.cpp
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 对EPoll封装的实现
 * @version 0.1
 * @date 2024-07-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "net/EPoller.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <unistd.h>
using namespace neonet;

namespace {
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;
} // namespace
EPoller::EPoller(EventLoop *loop)
    : m_ownerLoop(loop), m_epollFd(::epoll_create1(EPOLL_CLOEXEC)),
      m_events(kInitEventListSize) {
  if (m_epollFd < 0) {
    std::cout << "Failed in epoll_create1";
  }
}

EPoller::~EPoller() { ::close(m_epollFd); }

void EPoller::epoll(ChannelList *activeChannels) {
  int numEvents = ::epoll_wait(m_epollFd, m_events.data(),
                               static_cast<int>(m_events.size()), -1);
  int savedErrno = errno;
  if (numEvents > 0) {
    std::cout << numEvents << " events happened";
    fillActiveChannels(numEvents, activeChannels);
    if (static_cast<size_t>(numEvents) == m_events.size()) {
      m_events.resize(m_events.size() * 2);
    }
  } else if (numEvents == 0) {
    std::cout << "nothing happened";
  } else {
    // error happens, log uncommon ones
    if (savedErrno != EINTR) {
      errno = savedErrno;
      std::cout << "EPollPoller::poll()";
    }
  }
}

void EPoller::fillActiveChannels(int numEvents,
                                 ChannelList *activeChannels) const {
  for (int i = 0; i < numEvents; ++i) {
    Channel *channel = static_cast<Channel *>(m_events[i].data.ptr);
    channel->set_revents(m_events[i].events);
    activeChannels->push_back(channel);
  }
}

void EPoller::updateChannel(Channel *channel) {
  const int index = channel->index();
  std::cout << "fd = " << channel->fd() << " events = " << channel->events()
            << " index = " << index;
  // 需要重新向红黑树中添加fd
  if (index == kNew || index == kDeleted) {
    if (index == kNew) {
      int fd = channel->fd();
      m_channels[fd] = channel;
    }
    channel->set_index(kAdded);
    update(EPOLL_CTL_ADD, channel);
  } else {
    // 如果当前不关注事件，从红黑树中删除fd
    if (channel->isNoneEvent()) {
      update(EPOLL_CTL_DEL, channel);
      channel->set_index(kDeleted);
    } else {
      update(EPOLL_CTL_MOD, channel);
    }
  }
}

void EPoller::removeChannel(Channel *channel) {
  int fd = channel->fd();
  std::cout << "fd = " << fd;
  m_channels.erase(fd);
  if (channel->index() == kAdded) {
    update(EPOLL_CTL_DEL, channel);
  }
  channel->set_index(kNew);
}

bool EPoller::hasChannel(Channel *channel) const {
  assertInLoopThread();
  auto it = m_channels.find(channel->fd());
  return (it != m_channels.end()) && (it->second == channel);
}

void EPoller::update(int operation, Channel *channel) {
  struct epoll_event event;
  memset(&event, 0, sizeof event);
  event.events = channel->events();
  event.data.ptr = channel;
  int fd = channel->fd();
  std::cout << "epoll_ctl op = " << operationToString(operation)
            << " fd = " << fd << " event = {" << channel->eventsToString()
            << "}";
  if (::epoll_ctl(m_epollFd, operation, fd, &event) < 0) {
    if (operation == EPOLL_CTL_DEL) {
      std::cout << "epoll_ctl op =" << operationToString(operation)
                << " fd =" << fd;
    } else {
      std::cout << "epoll_ctl op =" << operationToString(operation)
                << " fd =" << fd;
    }
  }
}

void EPoller::assertInLoopThread() const { m_ownerLoop->assertInLoopThread(); }

const char *EPoller::operationToString(int op) {
  switch (op) {
  case EPOLL_CTL_ADD:
    return "ADD";
  case EPOLL_CTL_DEL:
    return "DEL";
  case EPOLL_CTL_MOD:
    return "MOD";
  default:
    assert(false && "ERROR op");
    return "Unknown Operation";
  }
}
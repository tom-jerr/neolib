/**
 * @file Channel.cpp
 * @author lzy (lzy_cs_LN@163.com)
 * @brief
 * @version 0.1
 * @date 2024-07-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "net/Channel.h"
#include "net/EventLoop.h"
#include <cassert>
#include <iostream>
#include <string>
using namespace neonet;

Channel::Channel(EventLoop *loop, int fd) : m_loop(loop), m_fd(fd) {}

Channel::~Channel() {
  assert(!m_eventHandling);
  assert(!m_addedToLoop);
  if (m_loop->isInLoopThread()) {
    assert(!m_loop->hasChannel(this));
  }
}

void Channel::tie(const std::shared_ptr<void> &obj) {
  m_tie = obj;
  m_isTied = true;
}

void Channel::update() {
  m_addedToLoop = true;
  m_loop->updateChannel(this);
}

void Channel::remove() {
  assert(isNoneEvent());
  m_addedToLoop = false;
  m_loop->removeChannel(this);
}

void Channel::handleEvent() {
  std::shared_ptr<void> guard;
  if (m_isTied) {
    guard = m_tie.lock();
    if (guard) {
      handleEventWithGuard();
    }
  } else {
    handleEventWithGuard();
  }
}

void Channel::handleEventWithGuard() {
  m_eventHandling = true;
  std::cout << reventsToString();
  if ((m_revents & EPOLLHUP) && !(m_revents & EPOLLIN)) {
    if (m_closeCallback)
      m_closeCallback();
  }
  if (m_revents & EPOLLERR) {
    if (m_errorCallback)
      m_errorCallback();
  }
  if (m_revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
    if (m_readCallback)
      m_readCallback();
  }
  if (m_revents & EPOLLOUT) {
    if (m_writeCallback)
      m_writeCallback();
  }
  m_eventHandling = false;
}

std::string Channel::reventsToString() const {
  return eventsToString(m_fd, m_revents);
}

std::string Channel::eventsToString() const {
  return eventsToString(m_fd, m_events);
}

std::string Channel::eventsToString(int fd, int ev) {
  std::string evStr;
  evStr += std::to_string(fd) + ": ";
  if (ev & EPOLLIN)
    evStr += "IN ";
  if (ev & EPOLLPRI)
    evStr += "PRI ";
  if (ev & EPOLLOUT)
    evStr += "OUT ";
  if (ev & EPOLLHUP)
    evStr += "HUP ";
  if (ev & EPOLLRDHUP)
    evStr += "RDHUP ";
  if (ev & EPOLLERR)
    evStr += "ERR ";
  return evStr;
}
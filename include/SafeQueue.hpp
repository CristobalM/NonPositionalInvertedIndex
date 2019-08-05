//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_SAFEQUEUE_HPP
#define NONPOSINVIDX_SAFEQUEUE_HPP

#include <mutex>
#include <deque>

template <class T>
class SafeQueue{
private:
  std::deque<T> q;
  std::mutex m;
public:
  void push_back(T &t){
    auto lg = std::lock_guard(m);
    q.push_back(t);
  }
  void push_front(T &t){
    auto lg = std::lock_guard(m);
    q.push_front(t);
  }

  template <typename... Args>
  void emplace_back(Args&&... args){
    auto lg = std::lock_guard(m);
    q.emplace_back(std::forward<Args>(args)...);
  }

  template <typename... Args>
  void emplace_front(Args&&... args){
    auto lg = std::lock_guard(m);
    q.emplace_front(std::forward<Args>(args)...);
  }

  void pop_back(){
    auto lg = std::lock_guard(m);
    q.pop_back();
  }

  void pop_front(){
    auto lg = std::lock_guard(m);
    q.pop_front();
  }

  bool empty(){
    auto lg = std::lock_guard(m);
    return q.empty();
  }

  void clear(){
    auto lg = std::lock_guard(m);
    q.clear();
  }

  T& front(){
    auto lg = std::lock_guard(m);
    return q.front();
  }

  T& back(){
    auto lg = std::lock_guard(m);
    return q.back();
  }

  unsigned long size(){
    auto lg = std::lock_guard(m);
    return q.size();
  }

};
#endif //NONPOSINVIDX_SAFEQUEUE_HPP

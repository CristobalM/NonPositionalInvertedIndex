//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_CIRCULARQUEUE_HPP
#define NONPOSINVIDX_CIRCULARQUEUE_HPP


#include <array>
#include <cassert>

template<class T, uint32_t container_sz, class idx_type = uint>
class CircularQueue{
  std::array<T, container_sz> container;
  int front_idx;
  int back_idx;
  size_t e_count;

  inline idx_type prev(idx_type idx){
    return (container_sz + idx - 1) % container_sz;
  }

  inline idx_type next(idx_type idx){
    return (idx + 1) % container_sz;
  }

public:

  explicit CircularQueue() :  front_idx(0), back_idx(0), e_count(0){}

  inline void push(const T &e){
    assert(!full());
    container[back_idx] = e;
    back_idx = next(back_idx);
    e_count++;
  }

  T &front(){
    assert(!empty());
    return container[front_idx];
  }

  T &top(){
    return back();
  }



  T &back(){
    assert(!empty());
    return container[prev(back_idx)];
  }

  void pop(){
    pop_back();
  }

  void pop_back(){
    assert(!empty());
    back_idx = prev(back_idx);
    e_count--;
  }



  inline void pop_front(){
    assert(!empty());
    front_idx = next(front_idx);
    e_count--;
  }

  size_t size(){
    return e_count;
  }

  inline bool empty(){
    return size() == 0;
  }

  inline bool full(){
    return size() ==  container_sz;
  }
};

#endif //NONPOSINVIDX_CIRCULARQUEUE_HPP

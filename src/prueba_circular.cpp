//
// Created by Cristobal Miranda, 2019
//

#include <iostream>

#include "CircularQueue.hpp"
int main(){
  using uint = unsigned int;
  CircularQueue<uint, 100> cq;
  
  cq.push(1);
  cq.push(2);
  cq.push(3);
  cq.push(4);
  cq.push(5);
  cq.push(6);
  while(!cq.empty()){
    std::cout << cq.top() << " ";
    cq.pop();
  }
  std::cout << std::endl;
}
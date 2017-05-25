// Example program
#include <iostream>
#include <string>
#include <thread>

int main()
{
  int x=2;    
  std::thread t1([=]() {
      for(int i:{0,1,2,3}) 
       std::cout<<i<<":"<<x<<std::endl;
      });
  t1.join();
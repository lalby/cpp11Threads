#include <thread>
#include <mutex>
#include<iostream>
#include <chrono>

std::mutex countmtx;

std::chrono::milliseconds interval(250);

void printValue(int flag)
{
   int count=1;
   while(count++ <10)
    {
        
        if(flag) {
            {
            std::lock_guard<std::mutex> lg(countmtx);
            //countmtx.lock();
            std::cout<<"\n even "<<flag<< "thread id"<<std::this_thread::get_id();
            //countmtx.unlock();
            }
            std::this_thread::sleep_for(interval);
            
        }
        else
        {
            //countmtx.lock();
            {
            std::lock_guard<std::mutex> lg(countmtx);
            std::cout<<"\n odd "<<flag<< "thread id"<<std::this_thread::get_id();
            }
            //countmtx.unlock();
            std::this_thread::sleep_for(interval);
            
        }
        
    }
}


int main()
{
    printf("Hello World");
    std::thread t(printValue,0);
    std::thread t2(printValue,1);
    
    t.join();
    t2.join();
    

    return 0;
}



























#include <stdio.h>
#include <vector>
#include <thread>
#include <mutex>
#include<iostream>
#include <chrono>
#include <condition_variable>

std::mutex countmtx;
std::chrono::milliseconds interval(250);
std::condition_variable cv;
int i=0;

void printValue1(int flag)
{
    
    while(++flag < 10 )
    {
        std::unique_lock<std::mutex> ulk(countmtx);
        {
         if(flag==5)
         {
         i=1;    
         cv.notify_one();
         }
         std::cout<<"\n flag"<<flag<<std::this_thread::get_id();
        } 
    }
} 


void printValue2(int flag)
{
    std::unique_lock<std::mutex> ulk(countmtx);
    cv.wait(ulk);
    //if(cv.wait_for(ulk,chrono::seconds(1)==cv_status::timeout)
    std::cout<<"\n got signal ";
    while(++flag < 10 )
    {
        std::cout<<"\n flag="<<flag<<std::this_thread::get_id();
    }
} 




int main()
{
    printf("Hello World");
    std::thread t(printValue1,0);
    std::thread t2(printValue2,1);
    
    t.join();
    t2.join();
    

    return 0;
}



























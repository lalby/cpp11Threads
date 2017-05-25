// move value 
#include <iostream>
#include <string>
#include <thread>

class Mydata
{
    int x;
    public:
    Mydata(int n):x(n){}
    void getdata(){ std::cout<<x;}
};    


void processdata(std::unique_ptr<Mydata> pf_mydata)
{
    pf_mydata->getdata();
}

int main()
{
  int x=2;    
  std::thread t1([=]() {
      for(int i:{0,1,2,3}) 
       std::cout<<i<<":"<<x<<std::endl;
      });
  t1.join();
  
  std::unique_ptr<Mydata> p(new Mydata(5));
  p->getdata();
  
  std::thread t2(processdata, std::move(p));
  std::cout<<"in main";
  //p->getdata();
  t2.join();
}  
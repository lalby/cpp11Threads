#include<iostream>
#include <vector>
#include <queue>
#include<atomic>
#include<chrono>
#include<mutex>
#include<thread>
#include<csignal>
#include<condition_variable>



using namespace std;

sig_atomic_t signal_caught =0;

std::mutex logMutex;
void sigint_handler(int sig)
{
    signal_caught=1;
}

void logFnc(string text)
{
    logMutex.lock();
    std::cout<<text;
    logMutex.unlock();
}    

typedef void (*logFunction ) (string text);

class AbstractRequest
{
    public:
    virtual void setValue(int value)=0;
    virtual void process()=0;
    virtual void finish()=0;
    
};

class Request :public AbstractRequest
{
    int value;
    logFunction outFnc;
    public:
    void setValue(int value){ this->value=value;}
    void setOutput(logFunction fnc) { outFnc = fnc; }
    void process() { cout<<"processed";}
    void finish() { cout <<"finished";
    }    
};



class Worker
{
    condition_variable cv;
    mutex mtx;
    unique_lock<mutex> ulock;
    AbstractRequest * request;
    bool running;
    bool ready;
    public:
    Worker(){
        running =true; ready=false; 
        ulock=unique_lock<mutex> (mtx);
    }
        void run();
        void stop(){ running = false;}
        void setRequest(AbstractRequest *request ){
            this->request = request;
            ready=true;
        }
        void getCondition(condition_variable * &cv);
    
};


void Worker::getCondition(condition_variable * &cv)
{
    cv=&(this->cv);
}



class Dispatcher
{
    static queue<AbstractRequest *> requests;
    static queue<Worker *> Workers;
    static mutex requestMutex;
    static mutex workerMutex;
    
    static vector<Worker *> allWorkers;
    static vector<thread *> threads;
    
    public:
    static bool init(int Workers);
    static bool stop();
    static void addRequest(AbstractRequest *request);
    static bool addWorker(Worker *worker);
    
};

queue<AbstractRequest *> Dispatcher::requests;
queue<Worker *> Dispatcher::Workers;

mutex Dispatcher::requestMutex;
mutex Dispatcher::workerMutex;

vector<Worker *> Dispatcher::allWorkers;
vector <thread *> Dispatcher::threads;


bool Dispatcher::init(int workers)
{
    thread *t =0;
    Worker *w =0;
    for(int i=0;i<workers;++i)
    {
        w= new Worker;
        allWorkers.push_back(w);
        t=new thread(&Worker::run,w);
        threads.push_back(t);
    }
    return true;
}

bool Dispatcher::stop()
{
    for(int i=0;i<allWorkers.size();i++)
    {
        allWorkers[i]->stop();
    }
    for(int j=0;j<threads.size();++j)
    threads[j]->join();
}


void Dispatcher::addRequest(AbstractRequest * request)
{
    workerMutex.lock();
    if(!Workers.empty())
    {
        Worker * worker=Workers.front();
        worker->setRequest(request);
        condition_variable *cv;
        worker->getCondition(cv);
        cv->notify_one();
        Workers.pop();
        workerMutex.unlock();
    }
    else
    {
        workerMutex.unlock();
        requestMutex.lock();
        requests.push(request);
        requestMutex.unlock(); 
    }
}

bool Dispatcher::addWorker(Worker * worker)
{
    bool wait=true;
    requestMutex.lock();
    if(!requests.empty())
    {
        AbstractRequest * request = requests.front();
        worker->setRequest(request);
        requests.pop();
        wait=false;
        requestMutex.unlock();
    }
    else
    {
        requestMutex.unlock();
        workerMutex.lock();
        Workers.push(worker);
        workerMutex.unlock();
    }
    return wait;
}


void Worker::run()
{
    while(running)
    {
        if(ready)
        {
            ready=false;
            request->process();
            request->finish();
        }
        
        if(Dispatcher::addWorker(this))
        {
            while(!ready && running)
            {
                if(cv.wait_for(ulock,chrono::seconds(1))==cv_status::timeout)
                {
                    
                }
            }
        }
    
    }
}




int main()
{
    signal(SIGINT,&sigint_handler);
    Dispatcher::init(10);
    std::cout<<"Initialised ";
    int cycles=0;
    Request *rq=0;
    
    while(!signal_caught && cycles<50)
    {
        rq=new Request();
        rq->setValue(cycles);
        rq->setOutput(&logFnc);
        Dispatcher::addRequest(rq);
        cycles++;
    }
 
    this_thread::sleep_for(chrono::seconds(5));   
    Dispatcher::stop();
    
}    
    
    
    
    
    

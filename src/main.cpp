#include <iostream>
#include "../include/scheduler.h"
#include <thread>

extern thread_local Scheduler* activeScheduler;

int main() {
    Scheduler scheduler1;
    Scheduler scheduler2;

    scheduler1.spawn([]{
        std::cout<<"Fiber 0 starting..."<<std::endl;
        std::cout<<"Fiber 0 hogging CPU..."<<std::endl;
        for (long i=0;i<1000000000;i++) {}
        std::cout<<"Fiber 0 done"<<std::endl;
    }, 1);

    scheduler1.spawn([]{
        std::cout<<"Fiber 1 starting..."<<std::endl;
        std::cout<<"Fiber 1 resuming..."<<std::endl;
    }, 2);

    scheduler2.spawn([]{
        std::cout<<"Fiber 2 starting..."<<std::endl;
        std::cout<<"Fiber 2 resuming..."<<std::endl;
    }, 3);

    std::thread t1([&] { scheduler1.run(); });
    std::thread t2([&] { scheduler2.run(); });

    t1.join();
    t2.join();

    std::cout << "all fibers done"<<std::endl;
    return 0;
}
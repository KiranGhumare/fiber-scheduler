#include <iostream>
#include "../include/scheduler.h"

extern Scheduler* activeScheduler;

int main() {
    Scheduler scheduler;
    scheduler.spawn([]{
        std::cout<<"Fiber 0 starting..."<<std::endl;
        std::cout<<"Fiber 0 hogging CPU..."<<std::endl;
        for (long i=0;i<1000000000;i++) {}
        std::cout<<"Fiber 0 done"<<std::endl;
    }, 1);

    scheduler.spawn([]{
        std::cout<<"Fiber 1 starting..."<<std::endl;
        std::cout<<"Fiber 1 resuming..."<<std::endl;
    }, 2);

    scheduler.spawn([]{
        std::cout<<"Fiber 2 starting..."<<std::endl;
        std::cout<<"Fiber 2 resuming..."<<std::endl;
    }, 3);

    scheduler.run();

    std::cout << "all fibers done"<<std::endl;
    return 0;
}
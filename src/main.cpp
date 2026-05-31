#include <iostream>
#include "../include/scheduler.h"

extern Scheduler* activeScheduler;

int main() {
    Scheduler scheduler;
    scheduler.spawn([]{
        std::cout<<"Fiber 0 starting..."<<std::endl;
        activeScheduler->yield();
        std::cout<<"Fiber 0 resuming..."<<std::endl;
    });

    scheduler.spawn([]{
        std::cout<<"Fiber 1 starting..."<<std::endl;
        activeScheduler->yield();
        std::cout<<"Fiber 1 resuming..."<<std::endl;
    });

    scheduler.spawn([]{
        std::cout<<"Fiber 2 starting..."<<std::endl;
        activeScheduler->yield();
        std::cout<<"Fiber 2 resuming..."<<std::endl;
    });

    scheduler.run();

    std::cout << "all fibers done"<<std::endl;
    return 0;
}
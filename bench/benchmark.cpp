#include <iostream>
#include <chrono>
#include <thread>
#include "../include/scheduler.h"

extern thread_local Scheduler* activeScheduler;

void benchFiberContextSwitch() {
    const int n = 1000000;
    int count = 0;
    signal(SIGALRM, SIG_IGN);
    Scheduler scheduler;
    scheduler.spawn([&]{
        while (count < n) {
            count++;
            activeScheduler->yield();
        }
    }, 1);

    scheduler.spawn([&]{
        while (count < n) {
            count++;
            activeScheduler->yield();
        }
    }, 1);

    auto start = std::chrono::high_resolution_clock::now();
    scheduler.run();
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    std::cout<<"Fiber context switches: "<<n<<std::endl;
    std::cout<<"Total time: "<<ns<<" ns"<<std::endl;
    std::cout<<"Per switch: "<<ns/n<<" ns"<<std::endl;
}

void benchThreadContextSwitch() {
    const int n = 1000000;
    int count = 0;
    std::mutex mtx;
    std::condition_variable cv;
    bool turn = false;
    auto start = std::chrono::high_resolution_clock::now();
    std::thread t1([&]{
        for (int i=0;i<n/2;i++) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]{ return !turn; });
            count++;
            turn = true;
            cv.notify_one();
        }
    });
    std::thread t2([&]{
        for (int i=0;i<n/2;i++) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]{ return turn; });
            count++;
            turn = false;
            cv.notify_one();
        }
    });
    t1.join();
    t2.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    std::cout<<"Thread context switches: "<<n<<std::endl;
    std::cout<<"Total time: "<<ns<<" ns"<<std::endl;
    std::cout<<"Per switch: "<<ns/n<<" ns\n"<<std::endl;
}

int main() {
    std::cout<<"====== Fiber Scheduler Benchmarks ======"<<std::endl;
    benchFiberContextSwitch();
    benchThreadContextSwitch();
    return 0;
}


#pragma once
#include <queue>
#include <ucontext.h>
#include "fiber.h"

const size_t FIBER_STACK_SIZE = 64 * 1024;

class Scheduler {
public:
    Scheduler();
    ~Scheduler();

    void spawn(std::function<void()> fn);

    void run();
    void yield();
    static void signalHandler(int signal);

private:
    std::queue<Fiber*> readyQueue;
    ucontext_t context;
    Fiber* current;
    Fiber* createFiber(std::function<void()> fn);
    static void fiberEntry();
};
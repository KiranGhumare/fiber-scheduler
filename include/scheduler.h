#pragma once
#include <queue>
#include <ucontext.h>
#include "fiber.h"

const size_t FIBER_STACK_SIZE = 64 * 1024;

class Scheduler {
public:
    Scheduler();
    ~Scheduler();

    void spawn(std::function<void()> fn, int priority = 0);

    void run();
    void yield();
    static void signalHandler(int signal);

private:
    struct FiberComparator {
        bool operator()(Fiber* a, Fiber* b) {
            return a->priority < b->priority;
        }
    };
    std::priority_queue<Fiber*, std::vector<Fiber*>, FiberComparator> readyQueue;
    ucontext_t context;
    Fiber* current;
    Fiber* createFiber(std::function<void()> fn);
    static void fiberEntry();
};
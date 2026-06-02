#pragma once
#include <ucontext.h>
#include <functional>

enum class FiberState {
    READY,
    RUNNING,
    DONE
};

struct Fiber {
    ucontext_t context;
    char* stack;
    FiberState state;
    std::function<void()> fn;
    int priority;  // higher number = higher priority
};
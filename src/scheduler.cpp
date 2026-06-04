#include <iostream>
#include "../include/scheduler.h"
#include <sys/time.h>
#include <signal.h>

Scheduler * activeScheduler = nullptr;
const size_t STACK_SIZE = 64*1024;

Scheduler::Scheduler() : current(nullptr) {
    activeScheduler = this;
}

void Scheduler::spawn(std::function<void()> fn, int priority) {
    Fiber* fiber = new Fiber;
    fiber->stack = new char[STACK_SIZE];
    fiber->state = FiberState::READY;
    fiber->fn = fn;
    fiber->priority = priority;
    getcontext(&fiber->context);
    fiber->context.uc_stack.ss_sp = fiber->stack;
    fiber->context.uc_stack.ss_size = STACK_SIZE;
    fiber->context.uc_link = nullptr;
    makecontext(&fiber->context, Scheduler::fiberEntry, 0);
    readyQueue.push(fiber);
}

void Scheduler::yield() {
    readyQueue.push(current);
    current->state = FiberState::READY;
    swapcontext(&current->context, &context);
}

void Scheduler::run() {
    activeScheduler = this;
    struct sigaction sa;
    sa.sa_handler = Scheduler::signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, nullptr);

    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 10000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 10000;
    setitimer(ITIMER_REAL, &timer, nullptr);

    while (!readyQueue.empty()) {
        Fiber* fiber = readyQueue.pop();
        if (fiber == nullptr) continue;
        // readyQueue.pop();
        current = fiber;
        current->state = FiberState::RUNNING;
        swapcontext(&context, &current->context);
    }

    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, nullptr);
}

void Scheduler::fiberEntry() {
    activeScheduler->current->fn();
    activeScheduler->current->state = FiberState::DONE;
    swapcontext(&activeScheduler->current->context, &activeScheduler->context);
}

void Scheduler::signalHandler(int signal) {
    if (activeScheduler && activeScheduler->current) {
        activeScheduler->yield();
    }
}

Scheduler::~Scheduler() {
    Fiber* fiber;
    while ((fiber = readyQueue.pop()) != nullptr) {
        delete[] fiber->stack;
        delete fiber;
    }
}

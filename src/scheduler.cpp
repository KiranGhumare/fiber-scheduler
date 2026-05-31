#include <iostream>
#include "../include/scheduler.h"

Scheduler * activeScheduler = nullptr;
const size_t STACK_SIZE = 64*1024;

Scheduler::Scheduler() : current(nullptr) {
    activeScheduler = this;
}

void Scheduler::spawn(std::function<void()> fn) {
    Fiber* fiber = new Fiber;
    fiber->stack = new char[STACK_SIZE];
    fiber->state = FiberState::READY;
    fiber->fn = fn;
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
    while (readyQueue.size()) {
        Fiber* fiber = readyQueue.front();
        readyQueue.pop();
        current = fiber;
        current->state = FiberState::RUNNING;
        swapcontext(&context, &current->context);
    }
}

void Scheduler::fiberEntry() {
    activeScheduler->current->fn();
    activeScheduler->current->state = FiberState::DONE;
    swapcontext(&activeScheduler->current->context, &activeScheduler->context);
}

Scheduler::~Scheduler() {
    while (readyQueue.size()) {
        Fiber* fiber = readyQueue.front();
        readyQueue.pop();
        delete[] fiber->stack;
        delete fiber;
    }
}

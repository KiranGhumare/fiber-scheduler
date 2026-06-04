#pragma once
#include <atomic>
#include "fiber.h"

struct Node {
    Fiber* fiber;
    std::atomic<Node*> next;
    Node(Fiber* f): fiber(f), next(nullptr) {}
};

class LockFreeQueue {
    public:
        LockFreeQueue();
        void push(Fiber* f);
        Fiber* pop();
        bool empty();
    
    private:
        std::atomic<Node*> head;
        std::atomic<Node*> tail;
};
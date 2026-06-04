#include "../include/lockfree_queue.h"

LockFreeQueue::LockFreeQueue() {
    Node* dummy = new Node(nullptr);
    head.store(dummy);
    tail.store(dummy);
}

void LockFreeQueue::push(Fiber* f) {
    Node* temp = new Node(f);
    while (true) {
        Node* curr = tail.load();
        Node* nextEle = curr->next.load();
        if (nextEle == nullptr) {
            if (curr->next.compare_exchange_strong(nextEle, temp)) {
                tail.compare_exchange_strong(curr, temp);
                return;
            }
        }
        else {
            tail.compare_exchange_strong(curr, nextEle);
        }
    }
}

Fiber* LockFreeQueue::pop() {
    Node* temp = tail.load();
    while (true) {
        Node* curHead = head.load();
        Node* curTail = tail.load();
        Node* nextEle = curHead->next.load();

        if (curHead == curTail) {
            if (nextEle == nullptr) return nullptr;
            tail.compare_exchange_strong(curTail, nextEle);
        }
        else {
            Fiber* fiber = nextEle->fiber;
            if (head.compare_exchange_strong(curHead, nextEle)) {
                delete curHead;
                return fiber;
            }
        }
    }
}

bool LockFreeQueue::empty() {
    return head.load()->next.load() == nullptr;
}
# Fiber Scheduler

A userspace **M:N fiber scheduler** built from scratch in C++. Fibers are lightweight, cooperative execution contexts scheduled entirely in userspace — the OS kernel has no knowledge of them. Context switching, scheduling, and preemption are all implemented manually.

## What is a Fiber?

A fiber is a lightweight unit of execution with its own stack and CPU context. Unlike OS threads, fibers are not managed by the kernel — they are created, scheduled, and switched between entirely in userspace. This makes them significantly cheaper:

| | OS Thread | Fiber |
|---|---|---|
| Context switch | ~1-10μs (kernel crossing) | ~100-300ns (userspace) |
| Stack size | ~8MB (fixed) | 64KB (configurable) |
| Max count | ~thousands | ~millions |
| Scheduled by | OS kernel | You |

## Features

- **Cooperative scheduling** — fibers voluntarily yield control via `yield()`
- **Preemptive scheduling** — `SIGALRM` based timer forces context switches every 10ms, preventing CPU starvation
- **Priority scheduling** — higher priority fibers run first
- **Lock-free run queue** — Michael-Scott lock-free queue using `std::atomic` CAS operations, no mutex on the hot path

## Upcoming

- Multiple OS threads with work stealing (true M:N scheduling)
- `fiber_sleep(ms)` — fiber suspends itself and wakes after a timeout
- Benchmarks: context switch latency vs `std::thread`, throughput for 10K short-lived tasks

## Architecture
main()
→ Scheduler::spawn(fn, priority)   // create fiber, push to run queue
→ Scheduler::run()                 // start scheduling loop
→ pick highest priority fiber from LockFreeQueue
→ swapcontext(scheduler → fiber)
→ fiberEntry()             // trampoline
→ fiber->fn()         // user's code runs here
→ yield() or SIGALRM  // fiber pauses
→ swapcontext(fiber → scheduler)
→ pick next fiber...

## How Context Switching Works

Each fiber has its own stack (64KB, `mmap` allocated) and a `ucontext_t` struct that holds a snapshot of CPU registers (instruction pointer, stack pointer, general purpose registers). `swapcontext` saves the current CPU state into one context and loads another — execution jumps to wherever the target context was last paused.

## How Preemption Works

A `SIGALRM` signal is delivered every 10ms via `setitimer`. A signal handler calls `yield()` on the currently running fiber, forcing it off the CPU even if it never voluntarily yields. This prevents any fiber from starving others.

## Build

```bash
mkdir build && cd build
cmake ..
make
./fiber_scheduler
```

## Requirements

- C++17
- CMake 3.15+
- POSIX-compatible OS (Linux recommended, macOS supported with deprecation warnings)

## Project Structure

fiber-scheduler/
├── include/
│   ├── fiber.h            # Fiber struct, FiberState enum
│   ├── scheduler.h        # Scheduler class
│   └── lockfree_queue.h   # Lock-free Michael-Scott queue
├── src/
│   ├── main.cpp           # Entry point
│   ├── scheduler.cpp      # Scheduler implementation
│   └── lockfree_queue.cpp # Lock-free queue implementation
└── CMakeLists.txt

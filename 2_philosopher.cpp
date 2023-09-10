#include "philosopher.hpp"

#include <twist/test/inject_fault.hpp>

/**
idea: change order of locking of one of the philosophers:

order of locking

first philosopher: 1 fork, then 2 fork
second philosopher: 2 fork, then 3 fork
...
pre-last philosopher: k - 1 fork, then k fork
last philosopher: 1 fork, then k fork

Let's proof there is no deadlock.


Let's model situation, using wait-for graph.

It is bipartite directed graph.

Left part consists of threads.
Right part consists of mutexes.

When lock is blocked by thread, thread is connected to mutex.
When lock is acquired by thread, mutex is connected to thread.
When thread unlocks, there is no edge between mutex and thread.


Deadlock <=> there is cycle in this graph.

Proof:

1) deadlock -> cycle

Suppose there is deadlock, that means:

- Each process must be waiting for a resource which is being held by another
process, which in turn is waiting for the first process to release the resource.
In general, there is a set of waiting processes, P = {P1, P2, ..., PN},
such that P1 is waiting for a resource held by P2,
P2 is waiting for a resource held by P3 and so on until PN is waiting for a
resource held by P1.

- Only one process can use a resource at a time.

- A resource can only be released voluntarily by the process holding it.

- A process holding at least one resource is waiting to acquire additional
resources which are currently held by other processes.
**/

namespace dining {

Philosopher::Philosopher(Table &table, size_t seat)
    : table_(table), seat_(seat), left_fork_(table_.LeftFork(seat)),
      right_fork_(table_.RightFork(seat)) {}

void Philosopher::Eat() {
  AcquireForks();
  EatWithForks();
  ReleaseForks();
}

bool Philosopher::IsLast() const { return table_.ToRight(seat_) == 0; }

// Acquire left_fork_ and right_fork_
void Philosopher::AcquireForks() {
  // last philosopher changes the right and left order
  // to make correct order in circle
  if (IsLast()) {
    right_fork_.lock();
    left_fork_.lock();

    // other philosophers lock in right order
  } else {
    left_fork_.lock();
    right_fork_.lock();
  }
}

void Philosopher::EatWithForks() {
  table_.AccessPlate(seat_);
  // Try to provoke data race
  table_.AccessPlate(table_.ToRight(seat_));
  ++meals_;
}

// Release left_fork_ and right_fork_
void Philosopher::ReleaseForks() {
  // Your code goes here
  left_fork_.unlock();
  right_fork_.unlock();
}

void Philosopher::Think() {
  // Random pause or context switch
  twist::test::InjectFault();
}

} // namespace dining

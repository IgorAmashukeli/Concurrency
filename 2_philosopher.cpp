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

Deadlock definition.

7.4.1 Mutual Exclusion
Shared resources such as read-only files do not lead to deadlocks.
Unfortunately some resources, such as printers and tape drives, require
exclusive access by a single process. 7.4.2 Hold and Wait To prevent this
condition processes must be prevented from holding one or more resources while
simultaneously waiting for one or more others. There are several possibilities
for this: Require that all processes request all resources at one time. This can
be wasteful of system resources if a process needs one resource early in its
execution and doesn't need some other resource until much later. Require that
processes holding resources must release them before requesting new resources,
and then re-acquire the released resources along with the new ones in a single
new request. This can be a problem if a process has partially completed an
operation using a resource and then fails to get it re-allocated after releasing
it. Either of the methods described above can lead to starvation if a process
requires one or more popular resources. 7.4.3 No Preemption Preemption of
process resource allocations can prevent this condition of deadlocks, when it is
possible. One approach is that if a process is forced to wait when requesting a
new resource, then all other resources previously held by this process are
implicitly released, ( preempted ), forcing this process to re-acquire the old
resources along with the new resources in a single request, similar to the
previous discussion. Another approach is that when a resource is requested and
not available, then the system looks to see what other processes currently have
those resources and are themselves blocked waiting for some other resource. If
such a process is found, then some of their resources may get preempted and
added to the list of resources for which the process is waiting. Either of these
approaches may be applicable for resources whose states are easily saved and
restored, such as registers and memory, but are generally not applicable to
other devices such as printers and tape drives. 7.4.4 Circular Wait One way to
avoid circular wait is to number all resources, and to require that processes
request resources only in strictly increasing ( or decreasing ) order. In other
words, in order to request resource Rj, a process must first release all Ri such
that i >= j. One big challenge in this scheme is determining the relative
ordering of the different resources


That means, if there is no circular wait, there is no deadlock.
There is no circular wait:



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

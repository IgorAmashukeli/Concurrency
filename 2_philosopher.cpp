#include "philosopher.hpp"

#include <twist/test/inject_fault.hpp>

/**
idea: change order of locking of one of the philosophers
**/

namespace dining {

Philosopher::Philosopher(Table& table, size_t seat)
    : table_(table),
      seat_(seat),
      left_fork_(table_.LeftFork(seat)),
      right_fork_(table_.RightFork(seat)) {
}

void Philosopher::Eat() {
  AcquireForks();
  EatWithForks();
  ReleaseForks();
}

bool Philosopher::IsLast() const {
  return table_.ToRight(seat_) == 0;
}

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

}  // namespace dining

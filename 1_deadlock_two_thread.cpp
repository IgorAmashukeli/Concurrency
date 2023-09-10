#include <mutex>
#include <wheels/test/framework.hpp>

#include "../barrier.hpp"

// https://gitlab.com/Lipovsky/tinyfibers
#include <tf/sched/spawn.hpp>
#include <tf/sched/yield.hpp>
#include <tf/sync/mutex.hpp>
#include <tf/sync/wait_group.hpp>

/**

idea:

1) we create two mutexes, each one does a lock

2) however we do lock order inversion (we don't follow resource ordering)

3) therefore we get a deadlock

 yeild is used to do the switching
**/

using tf::Mutex;
using tf::Spawn;
using tf::WaitGroup;
using tf::Yield;

void TwoFibersDeadLock() {
  // Mutexes
  Mutex a;
  Mutex b;

  // Fibers

  auto first = [&] {
    // I am a Fiber
    std::lock_guard guard_a(a);
    Yield();
    std::lock_guard guard_b(b);
  };

  auto second = [&] {
    // I am a Fiber
    std::lock_guard guard_b(b);
    Yield();
    std::lock_guard guard_a(a);
  };

  // No deadlock with one fiber

  // No deadlock expected here
  // Run routine twice to check that
  // routine leaves mutexes in unlocked state

  Spawn(first).Join();
  Spawn(first).Join();

  // Same for `second`
  Spawn(second).Join();
  Spawn(second).Join();

  ReadyToDeadLock();

  // Deadlock with two fibers
  WaitGroup wg;
  wg.Spawn(first).Spawn(second).Wait();

  // We do not expect to reach this line
  FAIL_TEST("No deadlock =(");
}

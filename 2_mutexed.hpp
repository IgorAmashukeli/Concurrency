#pragma once

#include <mutex>
#include <twist/ed/stdlike/mutex.hpp>

//////////////////////////////////////////////////////////////////////

/*
 * Safe API for mutual exclusion
 *
 * Usage:
 *
 * Mutexed<std::vector<Apple>> apples;
 *
 * {
 *   auto owner_ref = apples->Acquire();
 *   owner_ref->push_back(Apple{});
 * }  // <- release ownership
 *
 */

/**
We create a class OwnerRef and class Mutex.
The first one stores the pointer to the object and guard_lock.

Guard lock will be destroyed at the same time the OwnerRef will be destroyed.

It means that the unlock will last until the owner ref exists.

Acquire creates this OwnerRef (calls constructor) for the object.

So this creates a mechanism for mutexed type of data.

**/

template <typename T, class Mutex = twist::ed::stdlike::mutex> class OwnerRef {
public:
  OwnerRef(T *object_ptr, Mutex &mutex)
      : object_ptr_(object_ptr), guard_(mutex) {}

  T *operator->() { return object_ptr_; }

private:
  T *object_ptr_;
  std::lock_guard<Mutex> guard_;
};

template <typename T, class Mutex = twist::ed::stdlike::mutex> class Mutexed {
  // Define your own OwnerRef

public:
  // https://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/
  template <typename... Args>
  explicit Mutexed(Args &&...args) : object_(std::forward<Args>(args)...) {}

  OwnerRef<T, Mutex> Acquire() {
    return OwnerRef(&object_, mutex_); // Your code goes here
  }

private:
  T object_;
  Mutex mutex_; // Guards access to object_
};

//////////////////////////////////////////////////////////////////////

template <typename T> auto Acquire(Mutexed<T> &object) {
  return object.Acquire();
}

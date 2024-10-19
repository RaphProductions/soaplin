#include <core/lock/spinlock.h>

void spinlock_acquire(spinlock* l) {
  while (__atomic_test_and_set(&l->locked, __ATOMIC_ACQUIRE)) {
    __asm__ volatile("pause");
  }
}

void spinlock_release(spinlock* l) {
  __atomic_clear(&l->locked, __ATOMIC_RELEASE);
}
#include <core/lock/spinlock.h>
#include <stdint.h>
#include <sys/panic.h>
#include <stddef.h>

void spinlock_acquire(spinlock* l) {
  uint64_t cnt;

  while (__atomic_test_and_set(&l->locked, __ATOMIC_ACQUIRE)) {
    if (cnt > 30000)
      panic(DEADLOCKED_KERNEL_COMPONENT, NULL);
    
    cnt++;

    __asm__ volatile("pause");
  }
}

void spinlock_release(spinlock* l) {
  __atomic_clear(&l->locked, __ATOMIC_RELEASE);
}
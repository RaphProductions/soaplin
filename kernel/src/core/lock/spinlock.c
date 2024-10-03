// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of the MIT License.

#include <stdint.h>

// Define the spinlock structure as an integer for atomic operations
typedef struct {
    int lock;
} spinlock;

// Atomic test-and-set implementation using x86_64 assembly
static inline int atomic_flag_test_and_set(int *ptr) {
    int old_value = 1;  // The value to set (1 indicates the lock is acquired)
    __asm__ volatile (
        "xchg %0, %1"      // Exchange the value atomically
        : "=r" (old_value), "+m" (*ptr)  // Output old_value, modify *ptr
        : "0" (old_value)  // Input the value to set (1)
        : "memory"         // Clobbers memory to prevent reordering
    );
    return old_value;      // Returns the old value: 0 if the lock was free, 1 if busy
}

// Atomic clear implementation using x86_64 assembly
static inline void atomic_flag_clear(int *ptr) {
    __asm__ volatile (
        "movl $0, %0"      // Set the lock to 0 (release)
        : "+m" (*ptr)      // Modify the memory location
        :                  // No input registers
        : "memory"         // Clobbers memory to prevent reordering
    );
}

// Acquire the spinlock
void spinlock_acquire(spinlock *sp) {
    if (!sp)
        return;

    // Spin until the lock is acquired (lock is set to 1)
    //while (atomic_flag_test_and_set(&sp->lock))
    //    __asm__ volatile("pause");  // Pause to reduce CPU power consumption while spinning
}

// Release the spinlock
void spinlock_release(spinlock *sp) {
    if (!sp)
        return;

    // Clear the lock (set to 0)
    //atomic_flag_clear(&sp->lock);
}

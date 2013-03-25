// See LICENSE for license details.

#ifndef _RISCV_ATOMIC_H
#define _RISCV_ATOMIC_H

#include "config.h"

typedef struct { volatile long val; } atomic_t;
typedef struct { atomic_t lock; } spinlock_t;
#define SPINLOCK_INIT {{0}}

#define mb() __sync_synchronize()

static inline void atomic_set(atomic_t* a, long val)
{
  a->val = val;
}

static inline long atomic_read(atomic_t* a)
{
  return a->val;
}

static inline long atomic_add(atomic_t* a, long inc)
{
#ifdef PK_ENABLE_ATOMICS
  return __sync_fetch_and_add(&a->val, inc);
#else
  long ret = atomic_read(a);
  atomic_set(a, ret + inc);
  return ret;
#endif
}

static inline long atomic_swap(atomic_t* a, long val)
{
#ifdef PK_ENABLE_ATOMICS
  return __sync_lock_test_and_set(&a->val, val);
#else
  long ret = atomic_read(a);
  atomic_set(a, val);
  return ret;
#endif
}

static inline void spinlock_lock(spinlock_t* lock)
{
  do
  {
    while (atomic_read(&lock->lock))
      ;
  } while (atomic_swap(&lock->lock, -1));
  mb();
}

static inline void spinlock_unlock(spinlock_t* lock)
{
  mb();
  atomic_set(&lock->lock,0);
}

#endif

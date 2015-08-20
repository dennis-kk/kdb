#include "db_spinlock.h"

/* TODO ±£Áô*/

void kdb_spinlock_init(kdb_spinlock_t* slock, atomic_counter_t value) {
    assert(slock);
    atomic_counter_set(&slock->default_value, value);
    atomic_counter_set(&slock->value, value);
}

void kdb_spinlock_lock(kdb_spinlock_t* slock, atomic_counter_t require, atomic_counter_t value) {
    assert(slock);
    while (value != atomic_counter_cas(&slock->value, require, value)) {
#       ifdef WIN32
        SwitchToThread();
#       else
        sched_yield();
#       endif /* WIN32*/
    }
}

void kdb_spinlock_unlock(kdb_spinlock_t* slock) {
    atomic_counter_set(&slock->value, slock->default_value);
}

#ifndef DB_SPINLOCK_H
#define DB_SPINLOCK_H

#include "db_internal.h"

/* TODO ±£¡Ù*/

void kdb_spinlock_init(kdb_spinlock_t* slock, atomic_counter_t value);
void kdb_spinlock_lock(kdb_spinlock_t* slock, atomic_counter_t require, atomic_counter_t value);
void kdb_spinlock_unlock(kdb_spinlock_t* slock);

#endif /* DB_SPINLOCK_H */

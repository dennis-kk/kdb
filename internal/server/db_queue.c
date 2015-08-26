/*
 * Copyright (c) 2014-2015, dennis wang
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "db_queue.h"

/**
 * 线程安全队列
 */
struct _queue_t {
    kringbuffer_t*   rb;    /* ringbuffer */
    int              size;  /* 最大元素数量 */
    atomic_counter_t count; /* 当前元素数量 */
    klock_t*         lock;  /* 锁 */
};

/**
 * 队列元素
 */
typedef struct _queue_item_t {
    void* object; /* 用户数据指针 */
} kdb_queue_item_t;

kdb_queue_t* kdb_queue_create(int size) {
    kdb_queue_t* kq = create(kdb_queue_t);
    memset(kq, 0, sizeof(kdb_queue_t));
    assert(kq);
    kq->rb = ringbuffer_create(size * sizeof(kdb_queue_item_t));
    assert(kq->rb);
    kq->lock = lock_create();
    assert(kq->lock);
    kq->size = size;
    return kq;
}

void kdb_queue_destroy(kdb_queue_t* kq) {
    assert(kq);
    if (kq->rb) {
        ringbuffer_destroy(kq->rb);
    }
    if (kq->lock) {
        lock_destroy(kq->lock);
    }
    destroy(kq);
}

int kdb_queue_push(kdb_queue_t* kq, void* ptr) {
    kdb_queue_item_t item;
    assert(kq);
    assert(ptr);
    item.object = ptr;
    lock_lock(kq->lock);
    if (sizeof(kdb_queue_item_t) != ringbuffer_write(kq->rb, (char*)&item, sizeof(kdb_queue_item_t))) {
        lock_unlock(kq->lock);
        return db_error_queue_full;
    }
    lock_unlock(kq->lock);
    atomic_counter_inc(&kq->count);
    return db_error_ok;
}

void* kdb_queue_pop(kdb_queue_t* kq) {
    kdb_queue_item_t item;
    assert(kq);
    lock_lock(kq->lock);
    if (sizeof(kdb_queue_item_t) != ringbuffer_read(kq->rb, (char*)&item, sizeof(kdb_queue_item_t))) {
        lock_unlock(kq->lock);
        return 0;
    }
    lock_unlock(kq->lock);
    atomic_counter_dec(&kq->count);
    return item.object;
}

int kdb_queue_get_count(kdb_queue_t* kq) {
    assert(kq);
    return kq->count;
}

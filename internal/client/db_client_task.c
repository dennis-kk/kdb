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

#include "db_client_task.h"

struct _client_task_t {
    int           type;
    kdb_task_cb_t cb;
    void*         ptr;
    char*         key;
    char**        keys;
    int           key_count;
    void*         value;
    int           value_bytes;
    void**        values;
    int*          values_bytes;
    int           noreply;
    uint32_t      flags;
    uint32_t      exptime;
    uint64_t      casid;
    int           ret_code;
    kcond_t*      cond;
    klock_t*      lock;
    volatile int  finish;
};

kdb_client_task_t* kdb_client_task_create(int type) {
    kdb_client_task_t* task = create(kdb_client_task_t);
    assert(task);
    memset(task, 0, sizeof(kdb_client_task_t));
    task->type = type;
    return task;
}

void kdb_client_task_destroy(kdb_client_task_t* task) {
    int i = 0;
    assert(task);
    if (task->keys) {
        for (i = 0; i < task->key_count; i++) {
            destroy(task->keys[i]);
        }
        destroy(task->keys);
    }
    if (task->values) {
        for (i = 0; i < task->key_count; i++) {
            destroy(task->values[i]);
        }
        destroy(task->values);
    }
    if (task->key) {
        destroy(task->key);
    }
    if (task->cond) {
        cond_destroy(task->cond);
    }
    if (task->lock) {
        lock_destroy(task->lock);
    }
    destroy(task);
}

int kdb_client_task_set_sync(kdb_client_task_t* task) {
    assert(task);
    task->cond = cond_create();
    assert(task->cond);
    task->lock = lock_create();
    assert(task->lock);
    return db_client_ok;
}

int kdb_client_task_check_sync(kdb_client_task_t* task) {
    assert(task);
    return (task->cond ? 1 : 0);
}

int kdb_client_task_get_ret_code(kdb_client_task_t* task) {
    assert(task);
    return task->ret_code;
}

int kdb_client_task_set_key(kdb_client_task_t* task, const char* key) {
    assert(task);
    assert(key);
    if (task->key) {
        destroy(task->key);
    }
    task->key = create_raw(strlen(key) + 1);
    assert(task->key);
    strcpy(task->key, key);
    return db_client_ok;
}

const char* kdb_client_task_get_key(kdb_client_task_t* task) {
    assert(task);
    return task->key;
}

int kdb_client_task_set_keys(kdb_client_task_t* task, const char** keys, int count) {
    int i = 0;
    assert(task);
    assert(keys);
    assert(count);
    assert(MAX_KEYS >= count);
    if (task->keys) {
        for (i = 0; i < task->key_count; i++) {
            if (task->keys[i]) {
                destroy(task->keys[i]);
            }
        }
        destroy(task->keys);
    }
    task->key_count = 0;
    task->keys = create_type_ptr_array(char, count);
    assert(task->keys);
    for (; i < count; i++) {
        task->keys[i] = create_raw(strlen(keys[i]) + 1);
        assert(task->keys[i]);
        strcpy(task->keys[i], keys[i]);
        task->key_count += 1;
    }
    return db_client_ok;
}

int kdb_client_task_set_value(kdb_client_task_t* task, const void* value, int size) {
    assert(task);
    assert(value);
    assert(size);
    if (task->value) {
        destroy(task->value);
    }
    task->value = create_type(void, size);
    assert(task->value);
    memcpy(task->value, value, size);
    task->value_bytes = size;
    return db_client_ok;
}

int kdb_client_task_set_noreply(kdb_client_task_t* task, int noreply) {
    assert(task);
    task->noreply = noreply;
    return db_client_ok;
}

int kdb_client_task_get_noreply(kdb_client_task_t* task) {
    assert(task);
    return task->noreply;
}

int kdb_client_task_set_flags(kdb_client_task_t* task, uint32_t flags) {
    assert(task);
    task->flags = flags;
    return db_client_ok;
}

uint32_t kdb_client_task_get_flags(kdb_client_task_t* task) {
    assert(task);
    return task->flags;
}

int kdb_client_task_set_exptime(kdb_client_task_t* task, uint32_t exptime) {
    assert(task);
    task->exptime = exptime;
    return db_client_ok;
}

uint32_t kdb_client_task_get_exptime(kdb_client_task_t* task) {
    assert(task);
    return task->exptime;
}

int kdb_client_task_set_casid(kdb_client_task_t* task, uint64_t casid) {
    assert(task);
    task->casid = casid;
    return db_client_ok;
}

uint64_t kdb_client_task_get_casid(kdb_client_task_t* task) {
    assert(task);
    return task->casid;
}

int kdb_client_task_set_cb(kdb_client_task_t* task, kdb_task_cb_t cb, void* ptr) {
    assert(task);
    assert(cb);
    task->cb  = cb;
    task->ptr = ptr;
    return db_client_ok;
}

int kdb_client_task_wait(kdb_client_task_t* task) {
    assert(task);
    if (!task->cond) {
        // TODO 任务类型错误，非阻塞
    }
    lock_lock(task->lock);
    if (!task->finish) {
        cond_wait(task->cond, task->lock);
    }
    if (task->finish) {
        lock_unlock(task->lock);
        return db_client_ok;
    }
    lock_unlock(task->lock);
    return db_client_ok;
}

int kdb_client_task_wait_ms(kdb_client_task_t* task, uint32_t ms) {
    assert(task);
    if (!task->cond) {
        // TODO 任务类型错误，非阻塞
    }
    lock_lock(task->lock);
    if (!task->finish) {
        cond_wait_ms(task->cond, task->lock, ms);
    }
    if (task->finish) {
        lock_unlock(task->lock);
        return db_client_ok;
    }
    lock_unlock(task->lock);
    return db_client_ok;
}

int kdb_client_task_signal(kdb_client_task_t* task) {
    assert(task);
    if (!task->cond) {
        // TODO 任务类型错误，非阻塞
    }
    lock_lock(task->lock);
    task->finish = 1;
    lock_unlock(task->lock);
    cond_signal(task->cond);
    return db_client_ok;
}

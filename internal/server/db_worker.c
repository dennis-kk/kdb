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

#include "db_worker.h"
#include "db_queue.h"
#include "db_task.h"

struct _worker_t {
    kthread_runner_t* runner;
    kdb_queue_t*      queue;
};

void worker_func(kthread_runner_t* runner) {
    kdb_task_t*   task   = 0;
    kdb_worker_t* worker = (kdb_worker_t*)thread_runner_get_params(runner);
    while (thread_runner_check_start(runner)) {
        task = (kdb_task_t*)kdb_queue_pop(worker->queue);
        while (task) {
            kdb_task_do_task(task);
            kdb_task_destroy(task);
            task = (kdb_task_t*)kdb_queue_pop(worker->queue);
        }
        thread_sleep_ms(1);
    }
}

kdb_worker_t* kdb_worker_create() {
    kdb_worker_t* worker = create(kdb_worker_t);
    assert(worker);
    memset(worker, 0, sizeof(kdb_worker_t));
    return worker;
}

void kdb_worker_destroy(kdb_worker_t* worker) {
    kdb_task_t* task = 0;
    assert(worker);
    if (worker->runner) {
        thread_runner_destroy(worker->runner);
    }
    if (worker->queue) {
        /* 执行所有未处理指令 */
        task = (kdb_task_t*)kdb_queue_pop(worker->queue);
        while (task) {
            kdb_task_do_task(task);
            kdb_task_destroy(task);
            task = (kdb_task_t*)kdb_queue_pop(worker->queue);
        }
        kdb_queue_destroy(worker->queue);
    }
    destroy(worker);
}

int kdb_worker_start(kdb_worker_t* worker) {
    assert(worker);
    worker->queue = kdb_queue_create(1024 * 128);
    assert(worker->queue);
    worker->runner = thread_runner_create(worker_func, worker);
    assert(worker->runner);
    if (error_ok != thread_runner_start(worker->runner, 0)) {
        return db_error_start_worker;
    }
    return db_error_ok;
}

int kdb_worker_stop(kdb_worker_t* worker) {
    assert(worker);
    if (worker->runner) {
        thread_runner_stop(worker->runner);
    }
    return db_error_ok;
}

int kdb_worker_wait_for_stop(kdb_worker_t* worker) {
    assert(worker);
    if (worker->runner) {
        thread_runner_join(worker->runner);
    }
    return db_error_ok;
}

int kdb_worker_push(kdb_worker_t* worker, void* ptr) {
    assert(worker);
    assert(ptr);
    return kdb_queue_push(worker->queue, ptr);
}

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

#include "db_client.h"
#include "db_queue.h"
#include "db_client_task.h"
#include "memcache_client_analyzer.h"

struct _client_t {
    kdb_queue_t*                pending_queue; /* 提交但未完成的任务 */
    kdb_queue_t*                ret_queue;     /* 已经完成的任务 */
    kdb_queue_t*                exec_queue;    /* 正在执行的任务 */
    kloop_t*                    net_loop;      /* 网络循环 */
    kchannel_ref_t*             srv_channel;   /* 服务器管道 */
    kthread_runner_t*           worker;        /* 异步线程 */
    memcache_client_analyzer_t* analyzer;      /* 协议解析器 */
};

void connector_cb(kchannel_ref_t* channel, knet_channel_cb_event_e e) {
    int                error  = db_client_ok;
    kdb_client_t*      client = (kdb_client_t*)knet_channel_ref_get_ptr(channel);
    kdb_client_task_t* task   = 0;
    if (e & channel_cb_event_recv) {
        /* 解析协议并处理task */
        error = memcache_client_analyzer_analyze(client->analyzer, channel);
        if (db_client_ok == error) {
            task = kdb_queue_pop(client->exec_queue);
            if (task) {
                memcache_client_analyzer_do_task(client->analyzer, task);
            }
        }
    }
}

void worker_func(kthread_runner_t* runner) {
    kdb_client_t*      client = (kdb_client_t*)thread_runner_get_params(runner);
    kdb_client_task_t* task   = 0;
    while (thread_runner_check_start(runner)) {
        /* 运行网络循环 */
        knet_loop_run_once(client->net_loop);
        task = (kdb_client_task_t*)kdb_queue_pop(client->pending_queue);
        if (task) {
            /* 发送并压入exec_queue */
            kdb_client_do_task(client, task);
        }
    }
}

kdb_client_t* kdb_client_create() {
    kdb_client_t* client = create(kdb_client_t);
    assert(client);
    memset(client, 0, sizeof(kdb_client_t));
    client->net_loop = knet_loop_create();
    assert(client->net_loop);
    client->worker = thread_runner_create(worker_func, client);
    assert(client->worker);
    client->pending_queue = kdb_queue_create(1024 * 128);
    assert(client->pending_queue);
    client->exec_queue = kdb_queue_create(1024 * 128);
    assert(client->exec_queue);
    client->ret_queue  = kdb_queue_create(1024 * 128);
    assert(client->ret_queue);
    client->analyzer = memcache_client_analyzer_create();
    assert(client->analyzer);
    client->srv_channel = knet_loop_create_channel(client->net_loop, 1024, 1024 * 1024 * 2);
    knet_channel_ref_set_cb(client->srv_channel, connector_cb);
    knet_channel_ref_set_ptr(client->srv_channel, client);
    assert(client->srv_channel);
    if (error_ok != thread_runner_start_loop(client->worker, client->net_loop, 0)) {
        kdb_client_destroy(client);
        return 0;
    }
    return client;
}

void kdb_client_destroy(kdb_client_t* client) {
    assert(client);
    if (client->worker) {
        thread_runner_destroy(client->worker);
    }
    if (client->pending_queue) {
        kdb_queue_destroy(client->pending_queue);
    }
    if (client->ret_queue) {
        kdb_queue_destroy(client->ret_queue);
    }
    if (client->exec_queue) {
        kdb_queue_destroy(client->exec_queue);
    }
    if (client->net_loop) {
        knet_loop_destroy(client->net_loop);
    }
    if (client->analyzer) {
        memcache_client_analyzer_destroy(client->analyzer);
    }
    destroy(client);
}

void kdb_client_run(kdb_client_t* client) {
    kdb_client_task_t* task = 0;
    assert(client);
    while (thread_runner_check_start(client->worker)) {
        task = (kdb_client_task_t*)kdb_queue_pop(client->ret_queue);
        if (!task) {
            thread_sleep_ms(1);
            continue;
        }
        /* TODO 处理返回 */
    }
}

void kdb_client_run_once(kdb_client_t* client) {
    kdb_client_task_t* task = 0;
    assert(client);
    task = (kdb_client_task_t*)kdb_queue_pop(client->ret_queue);
    if (!task) {
        return;
    }
    /* TODO 处理返回 */
}

void kdb_client_stop(kdb_client_t* client) {
    assert(client);
    assert(client->worker);
    thread_runner_stop(client->worker);
}

void kdb_client_wait_for_stop(kdb_client_t* client) {
    assert(client);
    assert(client->worker);
    thread_runner_join(client->worker);
}

int kdb_client_exec(kdb_client_t* client, kdb_client_task_t* task) {
    assert(client);
    assert(task);
    kdb_queue_push(client->pending_queue, task);
    return db_client_ok;
}

int kdb_client_do_task(kdb_client_t* client, kdb_client_task_t* task) {
    kstream_t* stream = 0;
    assert(client);
    assert(task);
    stream = knet_channel_ref_get_stream(client->srv_channel);
    switch (kdb_client_task_get_type(task)) {
    case command_type_set:
        knet_stream_push_varg(
            stream, "set %s %d %d %d\r\n",
            kdb_client_task_get_key(task),
            kdb_client_task_get_flags(task),
            kdb_client_task_get_value_size(task));
        knet_stream_push(stream, kdb_client_task_get_value(task),
            kdb_client_task_get_value_size(task));
        knet_stream_push(stream, "\r\n", 2);
        break;
    }
}

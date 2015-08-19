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

#include "db_server.h"
#include "db_space.h"
#include "memcache_analyzer.h"

db_server_t*   db_server   = 0;
kloop_t*       server_loop = 0;
ktimer_loop_t* timer_loop  = 0;
db_space_t*    root_space  = 0;

struct _server_t {
    kthread_runner_t* runner;
    char              ip[32];
    int               port;
    int               root_space_buckets;
    int               space_buckets;
    int               channel_timeout;
    char*             action_buffer;
    int               timer_freq;
    int               timer_slot;
};

db_server_t* kdb_server_create() {
    db_server_t* srv = create(db_server_t);
    memset(srv, 0, sizeof(db_server_t));
    assert(srv);
    srv->action_buffer = create_raw(ACTION_BUFFER_LENGTH);
    assert(srv->action_buffer);
    return srv;
}

void kdb_server_destroy(db_server_t* srv) {
    assert(srv);
    if (server_loop) {
        knet_loop_destroy(server_loop);
    }
    if (timer_loop) {
        ktimer_loop_destroy(timer_loop);
    }
    if (root_space) {
        kdb_space_destroy(root_space);
    }
    if (srv->action_buffer) {
        destroy(srv->action_buffer);
    }
    destroy(srv);
}

/* TODO 命令行参数 */
int kdb_server_start(db_server_t* srv, int argc, char** argv) {
    kchannel_ref_t* acceptor = 0;
    (void)argc;
    (void)argv;
    assert(srv);
    /* 默认值 */
    srv->root_space_buckets = ROOT_SPACE_DEFAULT_BUCKETS;
    srv->space_buckets      = SPACE_DEFAULT_BUCKETS;
    srv->port               = SERVER_DEFAULT_PORT;
    srv->channel_timeout    = CHANNEL_DEFAULT_TIMEOUT;
    srv->timer_freq         = TIMER_DEFAULT_FREQ;
    srv->timer_slot         = TIMER_DEFAULT_SLOT;
    strcpy(srv->ip, SERVER_DEFAULT_IP);
    /* 建立根空间 */
    root_space = kdb_space_create(0, srv, srv->root_space_buckets);
    assert(root_space);
    /* 建立网络循环 */
    server_loop = knet_loop_create();
    assert(server_loop);
    /* 建立监听器 */
    acceptor = knet_loop_create_channel(server_loop, 128, ACTION_BUFFER_LENGTH);
    assert(acceptor);
    knet_channel_ref_set_cb(acceptor, acceptor_cb);
    if (error_ok != knet_channel_ref_accept(acceptor, srv->ip, srv->port, 1024)) {
        return db_error_listen_fail;
    }
    /* 建立定时器循环 */
    timer_loop = ktimer_loop_create(srv->timer_freq, srv->timer_slot);
    assert(timer_loop);
    /* 建立工作线程 */
    srv->runner = thread_runner_create(0, 0);
    assert(srv->runner);
    /* 启动工作线程 */
    if (error_ok != thread_runner_start_multi_loop_varg(srv->runner, 0, "lt", server_loop, timer_loop)) {
        return db_error_server_start_thread_fail;
    }
    return db_error_ok;
}

void kdb_server_stop(db_server_t* srv) {
    assert(srv);
    thread_runner_stop(srv->runner);
}

void kdb_server_wait_for_stop(db_server_t* srv) {
    assert(srv);
    thread_runner_join(srv->runner);
}

int kdb_server_get_space_buckets(db_server_t* srv) {
    assert(srv);
    return srv->space_buckets;
}

int kdb_server_get_channel_timeout(db_server_t* srv) {
    assert(srv);
    return srv->channel_timeout;
}

char* kdb_server_get_action_buffer(db_server_t* srv) {
    assert(srv);
    return srv->action_buffer;
}

int kdb_server_get_action_buffer_length(db_server_t* srv) {
    (void)srv;
    return ACTION_BUFFER_LENGTH;
}

void acceptor_cb(kchannel_ref_t* channel, knet_channel_cb_event_e e) {
    memcache_analyzer_t* mc = 0;
    if (e & channel_cb_event_accept) {
        /* 设置超时 */
        knet_channel_ref_set_timeout(channel, kdb_server_get_channel_timeout(db_server));
        /* 设置回调 */
        knet_channel_ref_set_cb(channel, connector_cb);
        /* 建立memcached兼容的协议解析器 */
        mc = memcache_analyzer_create();
        assert(mc);
        knet_channel_ref_set_ptr(channel, mc);
    }
}

void connector_cb(kchannel_ref_t* channel, knet_channel_cb_event_e e) {
    memcache_analyzer_t* mc = (memcache_analyzer_t*)knet_channel_ref_get_ptr(channel);
    if (e & channel_cb_event_recv) {
        if (!mc) {
            knet_channel_ref_close(channel);
            return;
        }
        /* 处理 */
        memcache_analyzer_drain(mc, channel);
    } else if (e & channel_cb_event_close) {
        if (mc) {
            memcache_analyzer_destroy(mc);
        }
        knet_channel_ref_set_ptr(channel, 0);
    }
}

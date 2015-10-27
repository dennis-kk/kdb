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

#include "db_server_node.h"
#include "db_queue.h"

struct _server_node_t {
    kchannel_ref_t* channel; /* 服务器管道 */
    kdb_queue_t*    tasks;   /* 任务队列 */
};

struct _server_node_mgr_t {
    kdb_server_node_t** nodes;      /* 服务节点数组 */
    int                 node_count; /* 服务节点数组长度 */
    int                 index;      /* 当前数量 */
};

kdb_server_node_t* kdb_server_node_create(const char* ip, int port) {
    kdb_server_node_t* srv_node = create(kdb_server_node_t);
    assert(srv_node);
    memset(srv_node, 0, sizeof(kdb_server_node_t));
    srv_node->tasks = kdb_queue_create(1024 * 64);
    assert(srv_node->tasks);
    return srv_node;
}

void kdb_server_node_destroy(kdb_server_node_t* srv_node) {
    assert(srv_node);
    if (srv_node->channel) {
        knet_channel_ref_decref(srv_node->channel);
    }
    if (srv_node->tasks) {
        /* TODO 销毁tasks */
    }
    destroy(srv_node);
}

void kdb_server_node_set_channel(kdb_server_node_t* srv_node, kchannel_ref_t* channel) {
    assert(srv_node);
    assert(channel);
    srv_node->channel = channel;
    knet_channel_ref_incref(channel);
}

uint32_t hash_string(const char* key) {
    uint32_t hash_key = 0;
    /* 简单的计算出整数key */
    for(; *key; key++) {
        hash_key = *key + hash_key * 31;
    }
    return hash_key;
}

int kdb_server_node_exec(kdb_server_node_t* srv_node, kdb_client_task_t* task) {
    assert(srv_node);
    assert(task);
    assert(srv_node->tasks);
    return kdb_queue_push(srv_node->tasks, task);
}

kdb_server_node_mgr_t* kdb_server_node_mgr_create() {
    kdb_server_node_mgr_t* mgr = create(kdb_server_node_mgr_t);
    assert(mgr);
    memset(mgr, 0, sizeof(kdb_server_node_mgr_t));
    return mgr;
}

void kdb_server_node_mgr_destroy(kdb_server_node_mgr_t* mgr) {
    int i = 0;
    assert(mgr);    
    if (mgr->nodes) {
        for (; i < mgr->node_count; i++) {
            if (mgr->nodes[i]) {
                destroy(mgr->nodes[i]);
            }
        }
        destroy(mgr->nodes);
    }
    destroy(mgr);
}

int kdb_server_node_mgr_add(kdb_server_node_mgr_t* mgr, kdb_server_node_t* snode) {
    assert(mgr);
    assert(snode);
    if (!mgr->nodes) {
        mgr->nodes = create_type_ptr_array(kdb_server_node_t, 8);
        memset(mgr->nodes, 0, 8 * sizeof(kdb_server_node_t*));
        mgr->nodes[0] = snode;
        mgr->node_count = 8;
        mgr->index = 1;
    } else {
        if (mgr->index >= mgr->node_count) {
            mgr->nodes = rcreate_type_ptr_array(kdb_server_node_t, mgr->nodes, mgr->node_count * 2);
            assert(mgr->nodes);
            mgr->node_count *= 2;
        }
        mgr->nodes[mgr->index] = snode;
        mgr->index += 1;
    }
    return db_client_ok;
}

int kdb_server_node_mgr_del(kdb_server_node_mgr_t* mgr, kdb_server_node_t* snode) {
    int i = 0;
    assert(mgr);
    assert(snode);
    for (; i < mgr->index; i++) {
        if (mgr->nodes[i] == snode) {
            destroy(mgr->nodes[i]);
            mgr->nodes[i] = 0;
        }
    }
    return db_client_ok;
}

kdb_server_node_t* kdb_server_node_mgr_balance(kdb_server_node_mgr_t* mgr, const char* key) {
    uint32_t hash_key = 0;
    assert(mgr);
    assert(key);
    hash_key = hash_string(key);
    return mgr->nodes[hash_key % (uint32_t)mgr->index];
}

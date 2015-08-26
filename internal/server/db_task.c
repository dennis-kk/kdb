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

#include "db_task.h"
#include "db_space.h"
#include "db_server.h"
#include "db_util.h"

/**
 * 任务
 */
struct _task_t {
    char**          keys;         /* 键 */
    int             key_count;    /* 键的数量 */
    char*           value;        /* 值 */
    int             value_bytes;  /* 值长度 */
    int             noreply;      /* noreply */
    uint32_t        flags;        /* flags */
    uint64_t        casid;        /* casid */
    uint32_t        exptime;      /* exptime */
    uint64_t        change_value; /* incr/decr value */
    int             task_type;    /* 任务类型 */
    kchannel_ref_t* channel;      /* 客户端管道 */
};

kdb_task_t* kdb_task_create(int task_type, kchannel_ref_t* channel) {
    kdb_task_t* task = kdb_create_type(kdb_task_t);
    assert(task);
    memset(task, 0, sizeof(kdb_task_t));
    task->task_type = task_type;
    task->channel   = channel;
    knet_channel_ref_incref(channel);
    return task;
}

void kdb_task_destroy(kdb_task_t* task) {
    int i = 0;
    assert(task);
    if (task->keys) {
        for (i = 0; i < task->key_count; i++) {
            kdb_free(task->keys[i]);
        }
        kdb_free(task->keys);
    }
    if (task->value) {
        kdb_free(task->value);
    }
    if (task->channel) {
        knet_channel_ref_decref(task->channel);
    }
    kdb_free(task);
}

void kdb_task_do_task(kdb_task_t* task) {
    int                i             = 0;
    int                error         = db_error_ok;
    kdb_space_t*       root_space    = kdb_server_get_root_space(db_server);
    kdb_space_value_t* sv            = 0;
    kdb_space_value_t* svs[MAX_KEYS] = {0};
    assert(task);
    switch (task->task_type) {
        case command_type_quit: /* quit */
            knet_channel_ref_close(task->channel);
            break;
        case command_type_set: /* set */
            error = kdb_space_set_key(root_space, task->keys[0], task->value, task->value_bytes,
                task->flags, task->exptime);
            break;
        case command_type_add: /* add */
            error = kdb_space_add_key(root_space, task->keys[0], task->value, task->value_bytes,
                task->flags, task->exptime);
            break;
        case command_type_addspace: /* addspace */
            error = kdb_space_add_space(root_space, task->keys[0], task->exptime);
            break;
        case command_type_replace: /* replace */
            error = kdb_space_update_key(root_space, task->keys[0], task->value,
                task->value_bytes, task->flags, task->exptime, 0);
            break;
        case command_type_cas: /* cas */
            error = kdb_space_cas_key(root_space, task->keys[0], task->value,
                task->value_bytes, task->flags, task->exptime, task->casid);
            break;
        case command_type_get: /* get */
            error = kdb_space_get_key(root_space, task->keys[0], &sv);
            break;
        case command_type_gets: /* gets */
            for (i = 0; i < task->key_count; i++) {
                error = kdb_space_get_key(root_space, task->keys[i], &svs[i]);
                if (db_error_ok != error) {
                    svs[i] = 0;
                }
            }
            error = db_error_ok;
            break;
        case command_type_delete: /* delete */
            error = kdb_space_del_key(root_space, task->keys[0]);
            break;
        case command_type_deletespace: /* deletespace */
            error = kdb_space_del_space(root_space, task->keys[0]);
            break;
        case command_type_subkey: /* subkey */
            error = kdb_space_subscribe_key(root_space, task->keys[0], task->channel);
            break;
        case command_type_sub: /* sub */
            error = kdb_space_subscribe(root_space, task->keys[0], task->channel);
            break;
        case command_type_leavekey: /* leavekey */
            error = kdb_space_forget_key(root_space, task->keys[0], task->channel);
            break;
        case command_type_leave: /* leave */
            error = kdb_space_forget(root_space, task->keys[0], task->channel);
            break;
        case command_type_incr: /* incr */
            error = kdb_space_incr_key(root_space, task->keys[0], task->change_value, &sv);
            break;
        case command_type_decr: /* decr */
            error = kdb_space_decr_key(root_space, task->keys[0], -1 * task->change_value, &sv);
            break;
        default:
            error = db_error_command_not_impl;
    }
    kdb_task_return(task, error, sv, svs);
}

void kdb_task_return_success(kdb_task_t* task, kdb_space_value_t* sv, kdb_space_value_t* svs[]) {
    int          i      = 0;
    kdb_value_t* v      = 0;
    kstream_t*   stream = knet_channel_ref_get_stream(task->channel);
    switch (task->task_type) {
        case command_type_set:
        case command_type_add:
        case command_type_addspace:
        case command_type_replace:
        case command_type_cas:
            knet_stream_push_varg(stream, STORED);
            break;
        case command_type_get:
            if (sv) {
                v = kdb_space_value_get_value(sv);
                knet_stream_push_varg(stream, VALUE_FORMAT, task->keys[0], kdb_value_get_cas_id(v), kdb_value_get_size(v));
                knet_stream_push(stream, kdb_value_get_value(v), kdb_value_get_size(v));
                knet_stream_push_varg(stream, MEMCACHED_CRLF);
            }
            knet_stream_push_varg(stream, END);
            break;
        case command_type_gets:
            for (i = 0; i < task->key_count; i++) {
                if (svs[i]) {
                    v = kdb_space_value_get_value(svs[i]);
                    knet_stream_push_varg(stream, VALUE_FORMAT, task->keys[i], kdb_value_get_cas_id(v), kdb_value_get_size(v));
                    knet_stream_push(stream, kdb_value_get_value(v), kdb_value_get_size(v));
                    knet_stream_push_varg(stream, MEMCACHED_CRLF);
                }
            }
            knet_stream_push_varg(stream, END);
            break;
        case command_type_incr:
        case command_type_decr:
            v = kdb_space_value_get_value(sv);
            knet_stream_push(stream, kdb_value_get_value(v), kdb_value_get_size(v));
            knet_stream_push_varg(stream, MEMCACHED_CRLF);
            break;
        case command_type_delete:
        case command_type_deletespace:
            knet_stream_push_varg(stream, DELETED);
            break;
        case command_type_subkey:
        case command_type_sub:
        case command_type_leavekey:
        case command_type_leave:
            knet_stream_push_varg(stream, STORED);
            break;
        default:
            break;
    }
}

void kdb_task_return_error(kdb_task_t* task, int error) {
    kstream_t* stream = knet_channel_ref_get_stream(task->channel);
    switch (task->task_type) {
        case command_type_set:
        case command_type_add:
        case command_type_addspace:
        case command_type_replace:
            knet_stream_push_varg(stream, NOT_STORED);
            break;
        case command_type_cas:
            if (db_error_cas_fail == error) {
                knet_stream_push_varg(stream, EXIST);
            } else {
                knet_stream_push_varg(stream, NOT_FOUND);
            }
            break;
        case command_type_get:
        case command_type_gets:
            knet_stream_push_varg(stream, END);
            break;
        case command_type_delete:
        case command_type_deletespace:
        case command_type_subkey:
        case command_type_sub:
        case command_type_leavekey:
        case command_type_leave:
            knet_stream_push_varg(stream, NOT_FOUND);
            break;
        case command_type_incr:
        case command_type_decr:
            knet_stream_push_varg(stream, NOT_FOUND);
            break;
        default:
            break;
    }
}

void kdb_task_return(kdb_task_t* task, int error, kdb_space_value_t* sv, kdb_space_value_t* svs[]) {
    kstream_t* stream = knet_channel_ref_get_stream(task->channel);
    if (task->noreply) {
        return;
    }
    switch (error) {
        case db_error_ok:
            kdb_task_return_success(task, sv, svs);
            break;
        case db_error_incr_decr_fail:
            knet_stream_push_varg(stream, SERVER_ERROR_FORMAT);
            break;
        default:
            kdb_task_return_error(task, error);
            break;
    }
}

void kdb_task_set_key(kdb_task_t* task, const char* key) {
    assert(task);
    assert(key);
    task->keys = create_type_ptr_array(char, 1);
    assert(task->keys);
    task->keys[0] = create_raw(strlen(key) + 1);
    assert(task->keys[0]);
    strcpy(task->keys[0], key);
    task->key_count = 1;
}

void kdb_task_set_value(kdb_task_t* task, void* data, int size) {
    assert(task);
    assert(data);
    assert(size);
    task->value = create_raw(size);
    assert(task->value);
    memcpy(task->value, data, size);
    task->value_bytes = size;
}

void kdb_task_set_keys(kdb_task_t* task, const char* keys[], int count) {
    int i = 0;
    assert(task);
    assert(keys);
    assert(count);
    task->keys = create_type_ptr_array(char, count);
    assert(task->keys);
    for (; i < count; i++) {
        task->keys[i] = create_raw(strlen(keys[i]) + 1);
        assert(task->keys[i]);
        strcpy(task->keys[i], keys[i]);
        task->key_count += 1;
    }
}

void kdb_task_space_set_key(kdb_task_t* task, const char* key, void* data, int size, uint32_t flags, uint32_t exptime) {
    assert(task);
    assert(key);
    assert(data);
    assert(size);
    kdb_task_set_key(task, key);
    kdb_task_set_value(task, data, size);
    task->flags   = flags;
    task->exptime = exptime;
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_add_key(kdb_task_t* task, const char* key, void* data, int size, uint32_t flags, uint32_t exptime) {
    assert(task);
    assert(key);
    assert(data);
    assert(size);
    kdb_task_set_key(task, key);
    kdb_task_set_value(task, data, size);
    task->flags   = flags;
    task->exptime = exptime;
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_add_space(kdb_task_t* task, const char* key, uint32_t exptime) {
    assert(task);
    assert(key);
    kdb_task_set_key(task, key);
    task->exptime = exptime;
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_update_key(kdb_task_t* task, const char* key, void* data, int size, uint32_t flags, uint32_t exptime) {
    assert(task);
    assert(key);
    assert(data);
    assert(size);
    kdb_task_set_key(task, key);
    kdb_task_set_value(task, data, size);
    task->flags   = flags;
    task->exptime = exptime;
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_cas_key(kdb_task_t* task, const char* key, void* data, int size, uint32_t flags, uint32_t exptime, uint64_t casid) {
    assert(task);
    assert(key);
    assert(data);
    assert(size);
    kdb_task_set_key(task, key);
    kdb_task_set_value(task, data, size);
    task->flags   = flags;
    task->exptime = exptime;
    task->casid   = casid;
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_get_key(kdb_task_t* task, const char* key) {
    assert(task);
    assert(key);
    kdb_task_set_key(task, key);
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_get_multi_key(kdb_task_t* task, const char** keys, int count) {
    assert(task);
    assert(keys);
    assert(count);
    kdb_task_set_keys(task, keys, count);
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_del_key(kdb_task_t* task, const char* key) {
    assert(task);
    assert(key);
    kdb_task_set_key(task, key);
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_del_space(kdb_task_t* task, const char* key) {
    assert(task);
    assert(key);
    kdb_task_set_key(task, key);
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_subscribe_key(kdb_task_t* task, const char* key) {
    assert(task);
    assert(key);
    kdb_task_set_key(task, key);
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_subscribe(kdb_task_t* task, const char* key) {
    assert(task);
    assert(key);
    kdb_task_set_key(task, key);
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_forget_key(kdb_task_t* task, const char* key) {
    assert(task);
    assert(key);
    kdb_task_set_key(task, key);
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_forget(kdb_task_t* task, const char* key) {
    assert(task);
    assert(key);
    kdb_task_set_key(task, key);
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_incr_key(kdb_task_t* task, const char* key, uint64_t change_value) {
    assert(task);
    assert(key);
    kdb_task_set_key(task, key);
    task->change_value = change_value;
    kdb_server_push_task(db_server, task);
}

void kdb_task_space_decr_key(kdb_task_t* task, const char* key, uint64_t change_value) {
    assert(task);
    assert(key);
    kdb_task_set_key(task, key);
    task->change_value = change_value;
    kdb_server_push_task(db_server, task);
}


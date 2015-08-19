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

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <assert.h>
#include <stdlib.h>
#include "knet.h"

typedef enum _db_error_e {
    db_error_ok = 0,
    db_error_invalid_path,
    db_error_exist,
    db_error_invalid_type,
    db_error_invalid_format,
    db_error_key_name_overflow,
    db_error_hash_insert,
    db_error_hash_delete,
    db_error_path_need_space_but_value,
    db_error_path_not_found,
    db_error_sub_fail,
    db_error_server_start_thread_fail,
    db_error_channel_fail,
    db_error_channel_rb_full,
    db_error_channel_need_more,
    db_error_unknown_command,
    db_error_command_not_impl,
    db_error_cas_fail,
    db_error_listen_fail,
} db_error_e;

typedef struct _server_t db_server_t;
typedef struct _space_t db_space_t;
typedef struct _value_t db_value_t;
typedef struct _memcache_analyzer_t memcache_analyzer_t;
typedef struct _db_space_value_t db_space_value_t;
typedef enum _db_space_value_type_e kdb_space_value_type_e;
typedef enum _db_sub_type_e kdb_sub_type_e;

#define for_each_char(c, s) \
    for (c = *s++; (c); c = *s++)

#define DOT                        '.'
#define MAX_NAME_SIZE              32
#define MAX_PATH_SIZE              MAX_NAME_SIZE * 4
#define ROOT_SPACE_DEFAULT_BUCKETS 10001
#define SPACE_DEFAULT_BUCKETS      129
#define SERVER_DEFAULT_IP          "192.168.2.40"
#define SERVER_DEFAULT_PORT        26888
#define TIMER_DEFAULT_FREQ         1000
#define TIMER_DEFAULT_SLOT         1000
#define CHANNEL_DEFAULT_TIMEOUT    5
#define ACTION_BUFFER_LENGTH       1024 * 1024
#define COMMAND_LINE_LENGTH        1024
#define COMMAND_LENGTH             32
#define FLAGS_LENGTH               32
#define EXPTIME_LENGTH             32
#define BYTES_LENGTH               32
#define NOREPLY_LENGTH             16
#define CAS_UNIQUE_LENGTH          64
#define MAX_KEYS                   128
#define CHANGE_VALUE_LENGTH        64
#define MEMCACHED_CRLF             "\r\n"

extern db_server_t*   db_server;     /* 服务器单件 */
extern db_space_t*    root_space;    /* 根空间 */
extern kloop_t*       server_loop;   /* 网络循环 */
extern ktimer_loop_t* timer_loop;    /* 定时器循环 */

#define EQUAL(a, b) \
    !strcmp(a, b)

#define EQUAL_RETURN(a, b, error) \
    do { \
        if (EQUAL(a, b)) { \
            return error; \
        } \
    } while(0);

#define NOT_EQUAL_RETURN(a, b, error) \
    do { \
        if (!EQUAL(a, b)) { \
            return error; \
        } \
    } while(0);

#define GET_FORWARD(buffer, key, size, pos, bytes, error) \
    do { \
        bytes = memcache_analyzer_command_line_get(buffer + pos, key, size); \
        if (!bytes) { \
            return error; \
        } \
        pos += bytes; \
    } while(0);

#define GET(buffer, key, size, pos, bytes, error) \
    do { \
        bytes = memcache_analyzer_command_line_get(buffer + pos, key, size); \
        if (!bytes) { \
            return error; \
        } \
    } while(0);

#endif /* SERVER_CONFIG_H */

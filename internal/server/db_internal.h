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

#ifndef DB_INTERNAL_H
#define DB_INTERNAL_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "knet.h"
#include "db_server_config.h"

typedef enum _db_sub_type_e         kdb_sub_type_e;
typedef struct _server_plugin_t     kdb_server_plugin_t;
typedef struct _memcache_analyzer_t memcache_analyzer_t;

#define DOT                        '.'                /* 路径分隔符 */
#define MAX_NAME_SIZE              32                 /* 子路径的最大名字长度 */
#define MAX_PATH_SIZE              MAX_NAME_SIZE * 4  /* 最大路径长度 */
#define ROOT_SPACE_DEFAULT_BUCKETS 10001              /* 根空间的哈希表默认桶数量 */
#define SPACE_DEFAULT_BUCKETS      129                /* 非空间的哈希表默认桶数量 */
#define SERVER_DEFAULT_IP          "192.168.2.40"     /* 默认IP */
#define SERVER_DEFAULT_PORT        26888              /* 默认监听端口 */
#define TIMER_DEFAULT_FREQ         1000               /* 定时器默认刷新频率(毫秒) */
#define TIMER_DEFAULT_SLOT         1000               /* 定时器默认时间轮槽位数量 */
#define CHANNEL_DEFAULT_TIMEOUT    5                  /* 客户端管道默认超时时间(秒) */
#define ACTION_BUFFER_LENGTH       1024 * 1024        /* 客户端一次发送的命令(包含值)的最大长度 */
#define COMMAND_LINE_LENGTH        1024               /* 单个命令行的最大长度(不包含值) */
#define COMMAND_LENGTH             32                 /* 命令最大长度 */
#define FLAGS_LENGTH               32                 /* memcached flags字段最大长度 */
#define EXPTIME_LENGTH             32                 /* memcached exptime字段最大长度 */
#define BYTES_LENGTH               32                 /* memcached bytes字段最大长度 */
#define NOREPLY_LENGTH             16                 /* memcached noreply字段最大长度 */
#define CAS_UNIQUE_LENGTH          64                 /* memcached CAS unique ID字段最大长度 */
#define MAX_KEYS                   128                /* gets命令最大键数量 */
#define CHANGE_VALUE_LENGTH        64                 /* memcached incr, decr值字段最大长度 */
#define MEMCACHED_CRLF             "\r\n"             /* memcached协议分割 */

extern kdb_server_t*  db_server;   /* 服务器单件 */
extern kdb_space_t*   root_space;  /* 根空间 */
extern kloop_t*       server_loop; /* 网络循环 */
extern ktimer_loop_t* timer_loop;  /* 定时器循环 */

#endif /* DB_INTERNAL_H */

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
#include <stddef.h>
#include <stdio.h>

#include "knet.h"
#include "db_server_config.h"

typedef enum   _db_sub_type_e       kdb_sub_type_e;
typedef struct _server_plugin_t     kdb_server_plugin_t;
typedef struct _memcache_analyzer_t memcache_analyzer_t;
typedef struct _queue_t             kdb_queue_t;
typedef struct _worker_t            kdb_worker_t;
typedef struct _task_t              kdb_task_t;

/**
 * 命令类型
 */
typedef enum _command_type_e {
    command_type_set = 1,     /* 设置值 */
    command_type_add,         /* 添加值 */
    command_type_addspace,    /* 新建空间 */
    command_type_replace,     /* 替换 */
    command_type_append,      /* 前添加 - 未实现 */
    command_type_prepend,     /* 后添加 - 未实现 */
    command_type_cas,         /* Check&Swap */
    command_type_get,         /* 获取单值 */ 
    command_type_gets,        /* 获取多值 */
    command_type_delete,      /* 销毁值 */
    command_type_deletespace, /* 销毁空间 */
    command_type_incr,        /* incr递增 */
    command_type_decr,        /* incr递减 */
    command_type_subkey,      /* 订阅值 */
    command_type_sub,         /* 订阅空间 */
    command_type_leavekey,    /* 取消订阅 - 值 */
    command_type_leave,       /* 取消订阅 - 空间 */
    command_type_quit,        /* 退出 */
} command_type_e;

#define MAX_NAME_SIZE              32                 /* 子路径的最大名字长度 */
#define PLUGIN_MAX_PATH            250                /* 插件路径最大长度 */
#define MAX_PATH_SIZE              MAX_NAME_SIZE * 4  /* 最大路径长度 */
#define ROOT_SPACE_DEFAULT_BUCKETS 10001              /* 根空间的哈希表默认桶数量 */
#define SPACE_DEFAULT_BUCKETS      129                /* 非根空间的哈希表默认桶数量 */
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
#define DOT                        '.'                /* 路径分隔符 */
#define SERVER_DEFAULT_IP          "0.0.0.0"          /* 默认IP */
#define MEMCACHED_CRLF             "\r\n"             /* memcached协议分割 */
#define MEMCACHED_NOREPLY          "noreply"          /* memcached noreply */
#define ROOT_SPACE                 "root"             /* 根空间名称 */
#define WHITE_SPACE                ' '                /* 空格 */
#define CR                         '\r'               /* 回车 */
#define SET                        "set"
#define SUBKEY                     "subkey"
#define SUB                        "sub"
#define ADD                        "add"
#define ADDSPACE                   "addspace"
#define APPEND                     "append"
#define LEAVEKEY                   "leavekey"
#define LEAVE                      "leave"
#define REPLACE                    "replace"
#define PREPEND                    "prepend"
#define CAS                        "cas"
#define GET                        "get"
#define GETS                       "gets"
#ifdef DELETE
#undef DELETE
#define DELETE                     "delete"
#endif /* DELETE */
#define DELETESPACE                "deletespace"
#define DECR                       "decr"
#define INCR                       "incr"
#define QUIT                       "quit"
#define CLIENT_ERROR_FORMAT        "CLIENT_ERROR (%s:%s)\r\n"
#define CLIENT_ERROR_FORMAT1        "CLIENT_ERROR (%s)\r\n"
#define INVALID_COMMAND_FORMAT     "invalid command format"
#ifdef ERROR
#undef ERROR
#define ERROR                      "ERROR\r\n"
#endif /* ERROR */
#define SERVER_ERROR_FORMAT1       "SERVER_ERROR (%s:%s)\r\n"
#define SERVER_ERROR_FORMAT2       "SERVER_ERROR (%s)\r\n"
#define SERVER_ERROR_FORMAT        "SERVER_ERROR\r\n"
#define COMMOND_NOT_IMPLEMENTED    "command not implemented"
#define NOT_FOUND                  "NOT_FOUND\r\n"
#define STORED                     "STORED\r\n"
#define END                        "END\r\n"
#define DELETED                    "DELETED\r\n"
#define NOT_STORED                 "NOT_STORED\r\n"
#define EXIST                      "EXIST\r\n"
#define VALUE_FORMAT               "VALUE %s %lld %d\r\n"
#define CRLF_END_CRLF              "\r\nEND\r\n"

/*! 未使用*/
#define UNUSED(v) (void)(v)
/*! 字符串遍历宏 */
#define for_each_char(c, s) for (c = *s++; (c); c = *s++)
/*! 分配type类型大小的内存 */
#define kdb_create_type(type) (type*)kdb_malloc(sizeof(type))
/*! 服务器单件 */
extern kdb_server_t* db_server;

#endif /* DB_INTERNAL_H */

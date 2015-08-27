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

#ifndef DB_COMMON_H
#define DB_COMMON_H

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

#define MAX_PATH_SIZE              MAX_NAME_SIZE * 4  /* 最大路径长度 */
#define COMMAND_LINE_LENGTH        1024               /* 单个命令行的最大长度(不包含值) */
#define COMMAND_LENGTH             32                 /* 命令最大长度 */
#define FLAGS_LENGTH               32                 /* memcached flags字段最大长度 */
#define EXPTIME_LENGTH             32                 /* memcached exptime字段最大长度 */
#define BYTES_LENGTH               32                 /* memcached bytes字段最大长度 */
#define NOREPLY_LENGTH             16                 /* memcached noreply字段最大长度 */
#define CAS_UNIQUE_LENGTH          64                 /* memcached CAS unique ID字段最大长度 */
#define MAX_KEYS                   128                /* gets命令最大键数量 */
#define CHANGE_VALUE_LENGTH        64                 /* memcached incr, decr值字段最大长度 */
#define CRLF                       "\r\n"
#define NOREPLY                    "noreply"
#define WHITE_SPACE                ' '
#define CR                         '\r'
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
#define KDB_DELETE                 "delete"
#define DELETESPACE                "deletespace"
#define DECR                       "decr"
#define INCR                       "incr"
#define QUIT                       "quit"

/*! 未使用*/
#define UNUSED(v) (void)(v)
/*! 字符串遍历宏 */
#define for_each_char(c, s) for (c = *s++; (c); c = *s++)

#endif /* DB_COMMON_H */

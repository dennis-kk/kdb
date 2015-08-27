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
#include "db_common.h"
#include "db_server_config.h"

typedef enum   _db_sub_type_e       kdb_sub_type_e;
typedef struct _server_plugin_t     kdb_server_plugin_t;
typedef struct _memcache_analyzer_t memcache_analyzer_t;
typedef struct _worker_t            kdb_worker_t;
typedef struct _task_t              kdb_task_t;

#define MAX_NAME_SIZE              32                 /* ��·����������ֳ��� */
#define PLUGIN_MAX_PATH            250                /* ���·����󳤶� */
#define ROOT_SPACE_DEFAULT_BUCKETS 10001              /* ���ռ�Ĺ�ϣ��Ĭ��Ͱ���� */
#define SPACE_DEFAULT_BUCKETS      129                /* �Ǹ��ռ�Ĺ�ϣ��Ĭ��Ͱ���� */
#define SERVER_DEFAULT_PORT        26888              /* Ĭ�ϼ����˿� */
#define TIMER_DEFAULT_FREQ         1000               /* ��ʱ��Ĭ��ˢ��Ƶ��(����) */
#define TIMER_DEFAULT_SLOT         1000               /* ��ʱ��Ĭ��ʱ���ֲ�λ���� */
#define CHANNEL_DEFAULT_TIMEOUT    5                  /* �ͻ��˹ܵ�Ĭ�ϳ�ʱʱ��(��) */
#define ACTION_BUFFER_LENGTH       1024 * 1024        /* �ͻ���һ�η��͵�����(����ֵ)����󳤶� */
#define DOT                        '.'                /* ·���ָ��� */
#define SERVER_DEFAULT_IP          "0.0.0.0"          /* Ĭ��IP */
#define ROOT_SPACE                 "root"             /* ���ռ����� */
#define CLIENT_ERROR_FORMAT        "CLIENT_ERROR (%s:%s)\r\n"
#define CLIENT_ERROR_FORMAT1       "CLIENT_ERROR (%s)\r\n"
#define INVALID_COMMAND_FORMAT     "invalid command format"
#define KDB_ERROR                  "ERROR\r\n"
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

/*! ����type���ʹ�С���ڴ� */
#define kdb_create_type(type) (type*)kdb_malloc(sizeof(type))
/*! ���������� */
extern kdb_server_t* db_server;

#endif /* DB_INTERNAL_H */

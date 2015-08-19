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

#define DOT                        '.'                /* ·���ָ��� */
#define MAX_NAME_SIZE              32                 /* ��·����������ֳ��� */
#define MAX_PATH_SIZE              MAX_NAME_SIZE * 4  /* ���·������ */
#define ROOT_SPACE_DEFAULT_BUCKETS 10001              /* ���ռ�Ĺ�ϣ��Ĭ��Ͱ���� */
#define SPACE_DEFAULT_BUCKETS      129                /* �ǿռ�Ĺ�ϣ��Ĭ��Ͱ���� */
#define SERVER_DEFAULT_IP          "192.168.2.40"     /* Ĭ��IP */
#define SERVER_DEFAULT_PORT        26888              /* Ĭ�ϼ����˿� */
#define TIMER_DEFAULT_FREQ         1000               /* ��ʱ��Ĭ��ˢ��Ƶ��(����) */
#define TIMER_DEFAULT_SLOT         1000               /* ��ʱ��Ĭ��ʱ���ֲ�λ���� */
#define CHANNEL_DEFAULT_TIMEOUT    5                  /* �ͻ��˹ܵ�Ĭ�ϳ�ʱʱ��(��) */
#define ACTION_BUFFER_LENGTH       1024 * 1024        /* �ͻ���һ�η��͵�����(����ֵ)����󳤶� */
#define COMMAND_LINE_LENGTH        1024               /* ���������е���󳤶�(������ֵ) */
#define COMMAND_LENGTH             32                 /* ������󳤶� */
#define FLAGS_LENGTH               32                 /* memcached flags�ֶ���󳤶� */
#define EXPTIME_LENGTH             32                 /* memcached exptime�ֶ���󳤶� */
#define BYTES_LENGTH               32                 /* memcached bytes�ֶ���󳤶� */
#define NOREPLY_LENGTH             16                 /* memcached noreply�ֶ���󳤶� */
#define CAS_UNIQUE_LENGTH          64                 /* memcached CAS unique ID�ֶ���󳤶� */
#define MAX_KEYS                   128                /* gets������������ */
#define CHANGE_VALUE_LENGTH        64                 /* memcached incr, decrֵ�ֶ���󳤶� */
#define MEMCACHED_CRLF             "\r\n"             /* memcachedЭ��ָ� */

extern kdb_server_t*  db_server;   /* ���������� */
extern kdb_space_t*   root_space;  /* ���ռ� */
extern kloop_t*       server_loop; /* ����ѭ�� */
extern ktimer_loop_t* timer_loop;  /* ��ʱ��ѭ�� */

#endif /* DB_INTERNAL_H */

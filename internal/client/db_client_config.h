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

#ifndef DB_CLIENT_CONFIG_H
#define DB_CLIENT_CONFIG_H

#include "knet.h"
#include "db_common.h"

typedef struct _client_t kdb_client_t;
typedef struct _client_task_t kdb_client_task_t;
typedef struct _server_node_t kdb_server_node_t;
typedef struct _server_node_mgr_t kdb_server_node_mgr_t;
typedef struct _memcache_client_analyzer_t memcache_client_analyzer_t;

typedef enum _client_error_e {
    db_client_ok = 0,
    db_client_task_timeout,
    db_client_need_more,
    db_client_invalid_format,
    db_client_unknown_command,
    db_client_command_error,
} kdb_client_error_e;

/*! 任务回调函数 */
typedef void (*kdb_task_cb_t)(kdb_client_task_t*);

#endif /* DB_CLIENT_CONFIG_H */

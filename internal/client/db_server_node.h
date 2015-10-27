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

#ifndef DB_SERVER_NODE_H
#define DB_SERVER_NODE_H

#include "db_client_config.h"

kdb_server_node_t* kdb_server_node_create(const char* ip, int port);
void kdb_server_node_destroy(kdb_server_node_t* srv_node);
void kdb_server_node_set_channel(kdb_server_node_t* srv_node, kchannel_ref_t* channel);
int kdb_server_node_exec(kdb_server_node_t* srv_node, kdb_client_task_t* task);
kdb_server_node_mgr_t* kdb_server_node_mgr_create();
void kdb_server_node_mgr_destroy(kdb_server_node_mgr_t* mgr);
int kdb_server_node_mgr_add(kdb_server_node_mgr_t* mgr, kdb_server_node_t* snode);
int kdb_server_node_mgr_del(kdb_server_node_mgr_t* mgr, kdb_server_node_t* snode);
kdb_server_node_t* kdb_server_node_mgr_balance(kdb_server_node_mgr_t* mgr, const char* key);

#endif /* DB_SERVER_NODE_H */

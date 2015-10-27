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

#ifndef DB_CLIENT_TASK_H
#define DB_CLIENT_TASK_H

#include "db_client_config.h"

kdb_client_task_t* kdb_client_task_create(int type);
void kdb_client_task_destroy(kdb_client_task_t* task);
int kdb_client_task_get_type(kdb_client_task_t* task);
int kdb_client_task_set_key(kdb_client_task_t* task, const char* key);
const char* kdb_client_task_get_key(kdb_client_task_t* task);
int kdb_client_task_set_keys(kdb_client_task_t* task, const char** keys, int count);
const char** kdb_client_task_get_keys(kdb_client_task_t* task);
int kdb_client_task_get_key_count(kdb_client_task_t* task);
int kdb_client_task_set_value(kdb_client_task_t* task, const void* value, int size);
const void* kdb_client_task_get_value(kdb_client_task_t* task);
int kdb_client_task_get_value_size(kdb_client_task_t* task);
int kdb_client_task_set_noreply(kdb_client_task_t* task, int noreply);
int kdb_client_task_get_noreply(kdb_client_task_t* task);
int kdb_client_task_set_flags(kdb_client_task_t* task, uint32_t flags);
uint32_t kdb_client_task_get_flags(kdb_client_task_t* task);
int kdb_client_task_set_exptime(kdb_client_task_t* task, uint32_t exptime);
uint32_t kdb_client_task_get_exptime(kdb_client_task_t* task);
int kdb_client_task_set_casid(kdb_client_task_t* task, uint64_t casid);
uint64_t kdb_client_task_get_casid(kdb_client_task_t* task);
int kdb_client_task_set_cb(kdb_client_task_t* task, kdb_task_cb_t cb, void* ptr);
int kdb_client_task_wait(kdb_client_task_t* task);
int kdb_client_task_wait_ms(kdb_client_task_t* task, uint32_t ms);
int kdb_client_task_signal(kdb_client_task_t* task);
int kdb_client_task_set_sync(kdb_client_task_t* task);
int kdb_client_task_get_ret_code(kdb_client_task_t* task);
int kdb_client_task_check_sync(kdb_client_task_t* task);

#endif /* DB_CLIENT_TASK_H  */

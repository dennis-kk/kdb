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

#ifndef DB_TASK_H
#define DB_TASK_H

#include "db_internal.h"

/**
 * 建立任务
 * @param task_type 任务类型
 * @param channel 客户端管道
 * @return kdb_task_t实例
 */
kdb_task_t* kdb_task_create(int task_type, kchannel_ref_t* channel);

/**
 * 销毁任务
 * @param task kdb_task_t实例
 */
void kdb_task_destroy(kdb_task_t* task);

/**
 * memcached set
 * @param task kdb_task_t实例
 * @param key 键
 * @param data 值
 * @param size 值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 */
void kdb_task_space_set_key(kdb_task_t* task, const char* key, void* data, int size, uint32_t flags, uint32_t exptime);

/**
 * memcached add
 * @param task kdb_task_t实例
 * @param key 键
 * @param data 值
 * @param size 值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 */
void kdb_task_space_add_key(kdb_task_t* task, const char* key, void* data, int size, uint32_t flags, uint32_t exptime);

/**
 * addspace
 * @param task kdb_task_t实例
 * @param key 键
 * @param exptime memcached exptime
 */
void kdb_task_space_add_space(kdb_task_t* task, const char* key, uint32_t exptime);

/**
 * memcached replace
 * @param task kdb_task_t实例
 * @param key 键
 * @param data 值
 * @param size 值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 */
void kdb_task_space_update_key(kdb_task_t* task, const char* key, void* data, int size, uint32_t flags, uint32_t exptime);

/**
 * memcached cas
 * @param task kdb_task_t实例
 * @param key 键
 * @param data 值
 * @param size 值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @param casid CAS ID
 */
void kdb_task_space_cas_key(kdb_task_t* task, const char* key, void* data, int size, uint32_t flags, uint32_t exptime, uint64_t casid);

/**
 * memcached get
 * @param task kdb_task_t实例
 * @param key 键
 */
void kdb_task_space_get_key(kdb_task_t* task, const char* key);

/**
 * memcached gets
 * @param task kdb_task_t实例
 * @param key 键数组
 * @param count 键数量
 */
void kdb_task_space_get_multi_key(kdb_task_t* task, const char** keys, int count);

/**
 * memcached delete
 * @param task kdb_task_t实例
 * @param key 键
 */
void kdb_task_space_del_key(kdb_task_t* task, const char* key);

/**
 * deletespace
 * @param task kdb_task_t实例
 * @param key 键
 */
void kdb_task_space_del_space(kdb_task_t* task, const char* key);

/**
 * subkey
 * @param task kdb_task_t实例
 * @param key 键
 */
void kdb_task_space_subscribe_key(kdb_task_t* task, const char* key);

/**
 * sub
 * @param task kdb_task_t实例
 * @param key 键
 */
void kdb_task_space_subscribe(kdb_task_t* task, const char* key);

/**
 * leavekey
 * @param task kdb_task_t实例
 * @param key 键
 */
void kdb_task_space_forget_key(kdb_task_t* task, const char* key);

/**
 * leave
 * @param task kdb_task_t实例
 * @param key 键
 */
void kdb_task_space_forget(kdb_task_t* task, const char* key);

/**
 * memcached incr
 * @param task kdb_task_t实例
 * @param key 键
 * @param change_value 改变的值
 */
void kdb_task_space_incr_key(kdb_task_t* task, const char* key, uint64_t change_value);

/**
 * memcached decr
 * @param task kdb_task_t实例
 * @param key 键
 * @param change_value 改变的值
 */
void kdb_task_space_decr_key(kdb_task_t* task, const char* key, uint64_t change_value);

/**
 * 执行任务
 * @param task kdb_task_t实例
 */
void kdb_task_do_task(kdb_task_t* task);

/**
 * 处理执行结果
 * @param task kdb_task_t实例
 * @param error 错误码
 * @param sv 单个返回值
 * @param svs 多个返回值
 */
void kdb_task_return(kdb_task_t* task, int error, kdb_space_value_t* sv, kdb_space_value_t* svs[]);

/**
 * 处理错误的执行结果
 * @param task kdb_task_t实例
 * @param error 错误码
 */
void kdb_task_return_error(kdb_task_t* task, int error);

/**
 * 处理成功的执行结果
 * @param task kdb_task_t实例
 * @param error 错误码
 * @param sv 单个返回值
 * @param svs 多个返回值
 */
void kdb_task_return_success(kdb_task_t* task, kdb_space_value_t* sv, kdb_space_value_t* svs[]);

#endif /* DB_TASK_H */

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

#ifndef DB_SERVER_API_H
#define DB_SERVER_API_H

#include "db_internal.h"

#ifdef __cplusplus /* C++ */
extern "C" {
#endif /* __cplusplus */

/**
 * 取得根空间
 * @param srv kdb_server_t实例
 * @return 根空间
 */
extern kdb_space_t* kdb_server_get_root_space(kdb_server_t* srv);

/**
 * 向空间内增加属性, 路径上不存在的空间也会自动建立
 * @param space kdb_space_t实例
 * @param path 路径
 * @param full_path 全路径
 * @param value 属性值
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
extern int kdb_space_set_key(kdb_space_t* space, const char* path, const char* full_path,
    const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * 向空间增加属性
 * @param space kdb_space_t实例
 * @param path 路径
 * @param full_path 全路径
 * @param value 属性值
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
extern int kdb_space_add_key(kdb_space_t* space, const char* path, const char* full_path,
    const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * 获取空间属性值
 * @param space kdb_space_t实例
 * @param path 路径
 * @param value 空间值指针返回
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
extern int kdb_space_get_key(kdb_space_t* space, const char* path, kdb_space_value_t** value);

/**
 * 销毁空间属性值
 * @param space kdb_space_t实例
 * @param path 路径
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
extern int kdb_space_del_key(kdb_space_t* space, const char* path);

/**
 * 更新属性值
 * @param space kdb_space_t实例
 * @param path 路径
 * @param value 属性值
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
extern int kdb_space_update_key(kdb_space_t* space, const char* path, const void* value,
    int size, uint32_t flags, uint32_t exptime);

/**
 * 实现memcache cas(check&set)功能
 * @param space kdb_space_t实例
 * @param path 路径
 * @param value 属性值
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @param cas_id memcached cas unique
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
extern int kdb_space_cas_key(kdb_space_t* space, const char* path, const void* value,
    int size, uint32_t flags, uint32_t exptime, uint64_t cas_id);

/**
 * 添加子空间
 * @param space kdb_space_t实例
 * @param path 路径
 * @param full_path 全路径
 * @param exptime memcached exptime
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
extern int kdb_space_add_space(kdb_space_t* space, const char* path, const char* full_path, uint32_t exptime);

/**
 * 获取子空间
 * @param space kdb_space_t实例
 * @param path 路径
 * @param child 子空间指针返回
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
extern int kdb_space_get_space(kdb_space_t* space, const char* path, kdb_space_t** child);

/**
 * 销毁子空间(包含所有属性)
 * @param space kdb_space_t实例
 * @param path 路径
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
extern int kdb_space_del_space(kdb_space_t* space, const char* path);

/**
 * 检查空间值类型(属性/子空间)
 * @param v kdb_space_value_t实例
 * @param type 类型
 * @retval 0 不是type类型
 * @retval 非零 是type类型
 */
extern int kdb_space_value_check_type(kdb_space_value_t* v, kdb_space_value_type_e type);

/**
 * 设置用户指针
 * @param v kdb_space_value_t实例
 * @param ptr 用户指针
 */
extern void kdb_space_value_set_ptr(kdb_space_value_t* v, void* ptr);

/**
 * 取得用户指针
 * @param v kdb_space_value_t实例
 * @return 用户指针
 */
extern void* kdb_space_value_get_ptr(kdb_space_value_t* v);

/**
 * 取得属性值
 * @param value kdb_space_value_t实例
 * @return kdb_value_t实例
 */
extern kdb_value_t* kdb_space_value_get_value(kdb_space_value_t* value);

/**
 * 取得值
 * @param value kdb_value_t实例
 * @return 值指针
 */
extern void* kdb_value_get_value(kdb_value_t* value);

/**
 * 取得值长度
 * @param value kdb_value_t实例
 * @return 值长度
 */
extern int kdb_value_get_size(kdb_value_t* value);

/**
 * 取得值memcached flags
 * @param value kdb_value_t实例
 * @return memcached flags
 */
extern uint32_t kdb_value_get_flags(kdb_value_t* value);

/**
 * 取得值memcached cas unique
 * @param value kdb_value_t实例
 * @return memcached cas unique
 */
extern uint64_t kdb_value_get_cas_id(kdb_value_t* value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DB_SERVER_API_H */

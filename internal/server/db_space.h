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

#ifndef DB_SPACE_H
#define DB_SPACE_H

#include "server_config.h"

/**
 * 建立空间
 * @param parent 父空间
 * @param srv 服务器
 * @param buckets 空间内哈希表桶的数量
 * @return db_space_t实例
 */
db_space_t* kdb_space_create(db_space_t* parent, db_server_t* srv, int buckets);

/**
 * 销毁空间
 * @param space db_space_t实例
 */
void kdb_space_destroy(db_space_t* space);

/**
 * 向空间内增加属性, 路径上不存在的空间也会自动建立
 * @param space db_space_t实例
 * @param path 路径
 * @param full_path 全路径
 * @param value 属性值
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_set_key(db_space_t* space, const char* path, const char* full_path, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * 向空间增加属性
 * @param space db_space_t实例
 * @param path 路径
 * @param full_path 全路径
 * @param value 属性值
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_add_key(db_space_t* space, const char* path, const char* full_path, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * 获取空间属性值
 * @param space db_space_t实例
 * @param path 路径
 * @param value 空间值指针返回
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_get_key(db_space_t* space, const char* path, db_space_value_t** value);

/**
 * 销毁空间属性值
 * @param space db_space_t实例
 * @param path 路径
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_del_key(db_space_t* space, const char* path);

/**
 * 更新属性值
 * @param space db_space_t实例
 * @param path 路径
 * @param value 属性值
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_update_key(db_space_t* space, const char* path, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * 实现memcache cas(check&set)功能
 * @param space db_space_t实例
 * @param path 路径
 * @param value 属性值
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @param cas_id memcached cas unique
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_cas_key(db_space_t* space, const char* path, const void* value, int size, uint32_t flags, uint32_t exptime, uint64_t cas_id);

/**
 * 订阅空间属性
 * @param space db_space_t实例
 * @param path 路径
 * @param channel 订阅的管道
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_subscribe_key(db_space_t* space, const char* path, kchannel_ref_t* channel);

/**
 * 订阅空间（包含子空间）内所有属性
 * @param space db_space_t实例
 * @param path 路径
 * @param channel 订阅的管道
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_subscribe(db_space_t* space, const char* path, kchannel_ref_t* channel);

/**
 * 取消订阅空间属性
 * @param space db_space_t实例
 * @param path 路径
 * @param channel 订阅的管道
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_forget_key(db_space_t* space, const char* path, kchannel_ref_t* channel);

/**
 * 取消订阅(包含子空间)
 * @param space db_space_t实例
 * @param path 路径
 * @param channel 订阅的管道
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_forget(db_space_t* space, const char* path, kchannel_ref_t* channel);

/**
 * 添加子空间
 * @param space db_space_t实例
 * @param path 路径
 * @param full_path 全路径
 * @param exptime memcached exptime
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_add_space(db_space_t* space, const char* path, const char* full_path, uint32_t exptime);

/**
 * 获取子空间
 * @param space db_space_t实例
 * @param path 路径
 * @param child 子空间指针返回
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_get_space(db_space_t* space, const char* path, db_space_t** child);

/**
 * 销毁子空间(包含所有属性)
 * @param space db_space_t实例
 * @param path 路径
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_del_space(db_space_t* space, const char* path);

/**
 * 获取服务器
 * @param space db_space_t实例
 * @return db_server_t指针
 */
db_server_t* kdb_space_get_server(db_space_t* space);

/**
 * 获取空间内哈希表桶的个数
 * @param space db_space_t实例
 * @return 哈希表桶的个数
 */
int kdb_space_get_buckets(db_space_t* space);

/**
 * 建立属性值
 * @param value 属性值指针
 * @param size 属性值长度
 * @return db_value_t实例
 */
db_value_t* value_create(const void* value, int size);

/**
 * 销毁属性值
 * @param value db_value_t实例
 */
void value_destroy(db_value_t* value);

/**
 * 取得值
 * @param value db_value_t实例
 * @return 值指针
 */
void* value_get_value(db_value_t* value);

/**
 * 取得值长度
 * @param value db_value_t实例
 * @return 值长度
 */
int value_get_size(db_value_t* value);

/**
 * 取得值memcached flags
 * @param value db_value_t实例
 * @return memcached flags
 */
uint32_t value_get_flags(db_value_t* value);

/**
 * 取得值memcached cas unique
 * @param value db_value_t实例
 * @return memcached cas unique
 */
uint64_t value_get_cas_id(db_value_t* value);

/**
 * 订阅属性
 * @param value db_value_t实例
 * @param chanenl 订阅的管道
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int value_subscribe(db_value_t* value, kchannel_ref_t* channel);

/**
 * 取消订阅属性
 * @param value db_space_value_t实例
 * @param chanenl 订阅的管道
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_value_forget(db_space_value_t* value, kchannel_ref_t* channel);

/**
 * 发布属性变化
 * @param value db_space_value_t实例
 * @param type 变化方式
 */
void kdb_space_value_publish(db_space_value_t* value, kdb_sub_type_e type);

/**
 * 取得属性值
 * @param value db_space_value_t实例
 * @return db_value_t实例
 */
db_value_t* kdb_space_value_get_value(db_space_value_t* value);

/**
 * 建立空间属性值
 * @param owner 所属空间
 * @param name 值名称
 * @param value 属性值指针
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @return db_space_value_t实例
 */
db_space_value_t* kdb_space_value_create_value(db_space_t* owner, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * 建立空间内子空间
 * @param owner 所属空间
 * @param full_path 全路径
 * @param exptime memcached exptime
 * @return db_space_value_t实例
 */
db_space_value_t* kdb_space_value_create_space(db_space_t* owner, const char* full_path, const char* name, uint32_t exptime);

/**
 * 销毁空间值
 * @param v db_space_value_t实例
 */
void kdb_space_value_destroy(db_space_value_t* v);

/**
 * 检查空间值类型(属性/子空间)
 * @param v db_space_value_t实例
 * @param type 类型
 * @retval 0 不是type类型
 * @retval 非零 是type类型
 */
int kdb_space_value_check_type(db_space_value_t* v, kdb_space_value_type_e type);

/**
 * 订阅空间内所有属性，包含子空间
 * @param v db_space_value_t实例
 * @param chanenl 订阅的管道
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_value_subscribe_space(db_space_value_t* v, kchannel_ref_t* channel);

/**
 * 取消订阅空间内所有属性，包含子空间
 * @param v db_space_value_t实例
 * @param chanenl 订阅的管道
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_value_forget_space(db_space_value_t* v, kchannel_ref_t* channel);

/**
 * 空间值哈希表销毁回调
 */
void value_dtor(void* v);

/**
 * 订阅管道哈希表销毁回调
 */
void sub_dtor(void* v);

/**
 * 路径字符串遍历函数
 * @param space 当前空间
 * @param path 路径
 * @param name 当前路径
 * @param name_len 当前路径长度
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_iterate_path(db_space_t* space, const char* path, char* name, int* name_len);

/**
 * 向空间内添加属性值, 如果路径不存在自动建立
 * @param space 当前空间
 * @param path 路径
 * @param full_path 全路径
 * @param name 当前路径
 * @param value 属性值指针
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_set_key_path(db_space_t* space, const char* path, const char* full_path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * 向空间内添加属性值
 * @param space 当前空间
 * @param path 路径
 * @param full_path 全路径
 * @param name 当前路径
 * @param value 属性值指针
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_add_key_path(db_space_t* space, const char* path, const char* full_path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * 获取空间内属性值
 * @param space 当前空间
 * @param path 路径
 * @param name 当前路径
 * @param value 空间值指针返回
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_get_key_path(db_space_t* space, const char* path, const char* name, db_space_value_t** value);

/**
 * 删除并销毁空间内属性值
 * @param space 当前空间
 * @param path 路径
 * @param name 当前路径
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_del_key_path(db_space_t* space, const char* path, const char* name);

/**
 * 更新空间内属性值
 * @param space 当前空间
 * @param path 路径
 * @param name 当前路径
 * @param value 属性值指针
 * @param size 属性值长度
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @param cas_id memcached cas unique
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_update_key_path(db_space_t* space, const char* path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime, uint64_t cas_id);

/**
 * 添加子孙空间
 * @param space 当前空间
 * @param path 路径
 * @param full_path 全路径
 * @param name 当前路径
 * @param exptime memcached exptime
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_add_space_path(db_space_t* space, const char* path, const char* full_path, const char* name, uint32_t exptime);

/**
 * 获取子孙空间
 * @param space 当前空间
 * @param path 路径
 * @param name 当前路径
 * @param child 子孙空间指针返回
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_get_space_path(db_space_t* space, const char* path, const char* name, db_space_t** child);

/**
 * 销毁子孙空间
 * @param space 当前空间
 * @param path 路径
 * @param name 当前路径
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_del_space_path(db_space_t* space, const char* path, const char* name);

/**
 * 订阅子孙空间
 * @param space 当前空间
 * @param path 路径
 * @param name 当前路径
 * @param channel 订阅管道
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_space_subscribe_space_path(db_space_t* space, const char* path, const char* name, kchannel_ref_t* channel);

#endif /* DB_SPACE_H */

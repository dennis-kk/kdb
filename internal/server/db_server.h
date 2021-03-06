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

#ifndef DB_SERVER_H
#define DB_SERVER_H

#include "db_internal.h"
#include "db_server_api.h"

/**
 * 建立服务器
 * @return kdb_server_t实例
 */
kdb_server_t* kdb_server_create();

/**
 * 销毁
 * @param srv kdb_server_t实例
 */
void kdb_server_destroy(kdb_server_t* srv);

/**
 * 启动
 * @param srv kdb_server_t实例
 * @param argc 参数个数
 * @param argv 参数指针数组
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_server_start(kdb_server_t* srv, int argc, char** argv);

/**
 * 停止
 * @param srv kdb_server_t实例
 */
void kdb_server_stop(kdb_server_t* srv);

/**
 * 等待停止
 * @param srv kdb_server_t实例
 */
void kdb_server_wait_for_stop(kdb_server_t* srv);

/**
 * 获取空间(非根空间)哈希表桶数量
 * @param srv kdb_server_t实例
 * @return 空间(非根空间)哈希表桶数量
 */
int kdb_server_get_space_buckets(kdb_server_t* srv);

/**
 * 获取客户端管道超时(秒)
 * @param srv kdb_server_t实例
 * @return 客户端管道超时(秒)
 */
int kdb_server_get_channel_timeout(kdb_server_t* srv);

/**
 * 获取命令缓冲区指针
 * @param srv kdb_server_t实例
 * @return 命令缓冲区指针
 */
char* kdb_server_get_action_buffer(kdb_server_t* srv);

/**
 * 获取命令缓冲区长度
 * @param srv kdb_server_t实例
 * @return 命令缓冲区长度
 */
int kdb_server_get_action_buffer_length(kdb_server_t* srv);

/**
 * 调用插件回调 - 新空间或属性值建立
 * @param srv kdb_server_t实例
 * @param value kdb_space_value_t实例
 */
void kdb_server_call_cb_on_add(kdb_server_t* srv, kdb_space_value_t* value);

/**
 * 调用插件回调 - 属性值删除
 * @param srv kdb_server_t实例
 * @param value kdb_space_value_t实例
 */
void kdb_server_call_cb_on_delete(kdb_server_t* srv, kdb_space_value_t* value);

/**
 * 调用插件回调 - 属性值更新
 * @param srv kdb_server_t实例
 * @param value kdb_space_value_t实例
 */
void kdb_server_call_cb_on_update(kdb_server_t* srv, kdb_space_value_t* value);

/**
 * 取得插件指针
 * @param srv kdb_server_t实例
 * @return kdb_server_plugin_t实例
 */
kdb_server_plugin_t* kdb_server_get_plugin(kdb_server_t* srv);

/**
 * 加载插件
 * @param srv kdb_server_t实例
 * @param file 插件文件名
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_server_load_plugin(kdb_server_t* srv, const char* file);

/**
 * 监听管道回调
 */
void acceptor_cb(kchannel_ref_t* channel, knet_channel_cb_event_e e);

/**
 * 客户端管道回调
 */
void connector_cb(kchannel_ref_t* channel, knet_channel_cb_event_e e);

/**
 * 输出帮助信息
 */
void kdb_server_print_help();

/**
 * 欢迎信息
 */
void kdb_server_welcome();

/**
 * 获取命令行参数
 * @param srv kdb_server_t实例
 * @param argc 参数个数
 * @param argv 参数指针数组
 */
void kdb_server_parse_command_line(kdb_server_t* srv, int argc, char** argv);

/**
 * 启动主线程
 * @param srv kdb_server_t实例
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_server_start_main(kdb_server_t* srv);

/**
 * 启动工作线程
 * @param srv kdb_server_t实例
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_server_start_worker(kdb_server_t* srv);

/**
 * 启动工作监听器
 * @param srv kdb_server_t实例
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_server_start_acceptor(kdb_server_t* srv);

/**
 * 获取malloc函数指针
 * @param srv kdb_server_t实例
 * @return malloc函数指针
 */
kdb_server_malloc_t kdb_server_get_malloc(kdb_server_t* srv);

/**
 * 获取realloc函数指针
 * @param srv kdb_server_t实例
 * @return realloc函数指针
 */
kdb_server_realloc_t kdb_server_get_realloc(kdb_server_t* srv);

/**
 * 获取free函数指针
 * @param srv kdb_server_t实例
 * @return free函数指针
 */
kdb_server_free_t kdb_server_get_free(kdb_server_t* srv);

/**
 * 添加到工作线程队列
 * @param srv kdb_server_t实例
 * @param task kdb_task_t实例
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int kdb_server_push_task(kdb_server_t* srv, kdb_task_t* task);

#endif /* DB_SERVER_H */

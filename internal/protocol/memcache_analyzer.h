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

#ifndef MEMCACHE_ANALYZER_H
#define MEMCACHE_ANALYZER_H

#include "server_config.h"

/**
 * 建立memcached协议解析器
 * @return memcache_analyzer_t实例
 */
memcache_analyzer_t* memcache_analyzer_create();

/**
 * 销毁memcached协议解析器
 * @param mc memcache_analyzer_t实例
 */
void memcache_analyzer_destroy(memcache_analyzer_t* mc);

/**
 * 重置解析器内属性
 * @param mc memcache_analyzer_t实例
 */
void memcache_analyzer_reset(memcache_analyzer_t* mc);

/**
 * 读取并处理memcached协议
 * @param mc memcache_analyzer_t实例
 * @param channel kchannel_ref_t实例
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_drain(memcache_analyzer_t* mc, kchannel_ref_t* channel);

/**
 * 处理memcached协议
 * @param mc memcache_analyzer_t实例
 * @param channel kchannel_ref_t实例
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_analyze(memcache_analyzer_t* mc, kchannel_ref_t* channel);

/**
 * 解析命令行 - incr, decr
 * @param mc memcache_analyzer_t实例
 * @param buffer 命令行
 * @param pos 解析的当前位置
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_do_analyze_incr_decr(memcache_analyzer_t* mc, const char* buffer, int pos);

/**
 * 解析命令行 - subkey, sub
 * @param mc memcache_analyzer_t实例
 * @param buffer 命令行
 * @param pos 解析的当前位置
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_do_analyze_sub(memcache_analyzer_t* mc, const char* buffer, int pos);

/**
 * 解析命令行 - leave, leavekey
 * @param mc memcache_analyzer_t实例
 * @param buffer 命令行
 * @param pos 解析的当前位置
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_do_analyze_leave(memcache_analyzer_t* mc, const char* buffer, int pos);

/**
 * 解析命令行 - delete, deletespace
 * @param mc memcache_analyzer_t实例
 * @param buffer 命令行
 * @param pos 解析的当前位置
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_do_analyze_delete(memcache_analyzer_t* mc, const char* buffer, int pos);

/**
 * 解析命令行 - get, gets
 * @param mc memcache_analyzer_t实例
 * @param buffer 命令行
 * @param pos 解析的当前位置
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_do_analyze_get(memcache_analyzer_t* mc, const char* buffer, int pos, int gets);

/**
 * 解析命令行 - 存储类命令
 * @param mc memcache_analyzer_t实例
 * @param buffer 命令行
 * @param pos 解析的当前位置
 * @param cas 是否是cas命令
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_do_analyze_store(memcache_analyzer_t* mc, const char* buffer, int pos, int cas);

/**
 * 解析命令行 - addspace
 * @param mc memcache_analyzer_t实例
 * @param buffer 命令行
 * @param pos 解析的当前位置
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_do_analyze_addspace(memcache_analyzer_t* mc, const char* buffer, int pos);

/**
 * 解析命令行
 * @param mc memcache_analyzer_t实例
 * @param buffer 命令行
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_analyze_command_line(memcache_analyzer_t* mc, const char* buffer);

/**
 * 解析命令行 - 获取单词
 * @param mc memcache_analyzer_t实例
 * @param command 命令行
 * @param size command长度
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_command_line_get(const char* s, char* command, int size);

/**
 * 执行命令
 * @param mc memcache_analyzer_t实例
 * @param channel kchannel_ref_t实例
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int memcache_analyzer_do_command(memcache_analyzer_t* mc, kchannel_ref_t* channel);

/**
 * 处理命令返回
 * @param mc memcache_analyzer_t实例
 * @param channel kchannel_ref_t实例
 * @param error 错误码
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
void memcache_analyzer_return(memcache_analyzer_t* mc, kchannel_ref_t* channel, int error);

/**
 * 发布更新事件
 * @param channel kchannel_ref_t实例
 * @param path 路径
 * @param dv db_space_value_t实例
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int publish_update(kchannel_ref_t* channel, const char* path, db_space_value_t* dv);

/**
 * 发布销毁事件
 * @param channel kchannel_ref_t实例
 * @param path 路径
 * @retval db_error_ok 成功
 * @retval 其他 失败
 */
int publish_delete(kchannel_ref_t* channel, const char* path);

#endif /* MEMCACHE_ANALYZER_H */

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

#ifndef DB_UTIL_H
#define DB_UTIL_H

#include "db_internal.h"

/**
 * 检查内存块是否为字符串描述的数字
 * @param s 内存块指针
 * @param size 内存块长度
 * @retval 0 不是
 * @retval 1 是
 */
int isnumber(void* s, int size);

/**
 * long long转换为字符串
 * @param ll long long
 * @param buffer 转换后存放的缓冲区
 * @param size 缓冲区长度
 * @return 转换后的字符串指针
 */
char* kdb_lltoa(long long ll, char* buffer, int* size);

/**
 * 字符串转long long
 * @param s 字符串
 * @param size 长度
 * @return long long
 */
long long atoll_s(void* s, int size);

/**
 * 获取可执行文件目录
 * @return 可执行文件目录
 */
const char* get_exe_path();

/**
 * 调用malloc或用户自定义malloc
 * @param size 需要分配的大小
 * @return 内存地址
 */
void* kdb_malloc(int size);

/**
 * 调用realloc或用户自定义realloc
 * @param p 原有的内存地址
 * @param size 需要分配的大小
 * @return 内存地址
 */
void* kdb_realloc(void* p, int size);

/**
 * 调用free或用户自定义free
 * @param p 内存地址
 */
void kdb_free(void* p);

#define EQUAL(a, b) \
    !strcmp(a, b)

#define EQUAL_RETURN(a, b, error) \
    do { \
        if (EQUAL(a, b)) { \
            return error; \
        } \
    } while(0);

#define NOT_EQUAL_RETURN(a, b, error) \
    do { \
        if (!EQUAL(a, b)) { \
            return error; \
        } \
    } while(0);

#define GET_FORWARD(buffer, key, size, pos, bytes, error) \
    do { \
        bytes = memcache_analyzer_command_line_get(buffer + pos, key, size); \
        if (!bytes) { \
            return error; \
        } \
        pos += bytes; \
    } while(0);

#define GET_HOLD(buffer, key, size, pos, bytes, error) \
    do { \
        bytes = memcache_analyzer_command_line_get(buffer + pos, key, size); \
        if (!bytes) { \
            return error; \
        } \
    } while(0);

#endif /* DB_UTIL_H */

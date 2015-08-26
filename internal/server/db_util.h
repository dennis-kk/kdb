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
 * ����ڴ���Ƿ�Ϊ�ַ�������������
 * @param s �ڴ��ָ��
 * @param size �ڴ�鳤��
 * @retval 0 ����
 * @retval 1 ��
 */
int isnumber(void* s, int size);

/**
 * long longת��Ϊ�ַ���
 * @param ll long long
 * @param buffer ת�����ŵĻ�����
 * @param size ����������
 * @return ת������ַ���ָ��
 */
char* kdb_lltoa(long long ll, char* buffer, int* size);

/**
 * �ַ���תlong long
 * @param s �ַ���
 * @param size ����
 * @return long long
 */
long long atoll_s(void* s, int size);

/**
 * ��ȡ��ִ���ļ�Ŀ¼
 * @return ��ִ���ļ�Ŀ¼
 */
const char* get_exe_path();

/**
 * ����malloc���û��Զ���malloc
 * @param size ��Ҫ����Ĵ�С
 * @return �ڴ��ַ
 */
void* kdb_malloc(int size);

/**
 * ����realloc���û��Զ���realloc
 * @param p ԭ�е��ڴ��ַ
 * @param size ��Ҫ����Ĵ�С
 * @return �ڴ��ַ
 */
void* kdb_realloc(void* p, int size);

/**
 * ����free���û��Զ���free
 * @param p �ڴ��ַ
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

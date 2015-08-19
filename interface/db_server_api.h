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
 * ȡ�ø��ռ�
 * @param srv kdb_server_tʵ��
 * @return ���ռ�
 */
extern kdb_space_t* kdb_server_get_root_space(kdb_server_t* srv);

/**
 * ��ռ�����������, ·���ϲ����ڵĿռ�Ҳ���Զ�����
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @param full_path ȫ·��
 * @param value ����ֵ
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
extern int kdb_space_set_key(kdb_space_t* space, const char* path, const char* full_path,
    const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * ��ռ���������
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @param full_path ȫ·��
 * @param value ����ֵ
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
extern int kdb_space_add_key(kdb_space_t* space, const char* path, const char* full_path,
    const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * ��ȡ�ռ�����ֵ
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @param value �ռ�ֵָ�뷵��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
extern int kdb_space_get_key(kdb_space_t* space, const char* path, kdb_space_value_t** value);

/**
 * ���ٿռ�����ֵ
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
extern int kdb_space_del_key(kdb_space_t* space, const char* path);

/**
 * ��������ֵ
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @param value ����ֵ
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
extern int kdb_space_update_key(kdb_space_t* space, const char* path, const void* value,
    int size, uint32_t flags, uint32_t exptime);

/**
 * ʵ��memcache cas(check&set)����
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @param value ����ֵ
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @param cas_id memcached cas unique
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
extern int kdb_space_cas_key(kdb_space_t* space, const char* path, const void* value,
    int size, uint32_t flags, uint32_t exptime, uint64_t cas_id);

/**
 * ����ӿռ�
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @param full_path ȫ·��
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
extern int kdb_space_add_space(kdb_space_t* space, const char* path, const char* full_path, uint32_t exptime);

/**
 * ��ȡ�ӿռ�
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @param child �ӿռ�ָ�뷵��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
extern int kdb_space_get_space(kdb_space_t* space, const char* path, kdb_space_t** child);

/**
 * �����ӿռ�(������������)
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
extern int kdb_space_del_space(kdb_space_t* space, const char* path);

/**
 * ���ռ�ֵ����(����/�ӿռ�)
 * @param v kdb_space_value_tʵ��
 * @param type ����
 * @retval 0 ����type����
 * @retval ���� ��type����
 */
extern int kdb_space_value_check_type(kdb_space_value_t* v, kdb_space_value_type_e type);

/**
 * �����û�ָ��
 * @param v kdb_space_value_tʵ��
 * @param ptr �û�ָ��
 */
extern void kdb_space_value_set_ptr(kdb_space_value_t* v, void* ptr);

/**
 * ȡ���û�ָ��
 * @param v kdb_space_value_tʵ��
 * @return �û�ָ��
 */
extern void* kdb_space_value_get_ptr(kdb_space_value_t* v);

/**
 * ȡ������ֵ
 * @param value kdb_space_value_tʵ��
 * @return kdb_value_tʵ��
 */
extern kdb_value_t* kdb_space_value_get_value(kdb_space_value_t* value);

/**
 * ȡ��ֵ
 * @param value kdb_value_tʵ��
 * @return ֵָ��
 */
extern void* kdb_value_get_value(kdb_value_t* value);

/**
 * ȡ��ֵ����
 * @param value kdb_value_tʵ��
 * @return ֵ����
 */
extern int kdb_value_get_size(kdb_value_t* value);

/**
 * ȡ��ֵmemcached flags
 * @param value kdb_value_tʵ��
 * @return memcached flags
 */
extern uint32_t kdb_value_get_flags(kdb_value_t* value);

/**
 * ȡ��ֵmemcached cas unique
 * @param value kdb_value_tʵ��
 * @return memcached cas unique
 */
extern uint64_t kdb_value_get_cas_id(kdb_value_t* value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DB_SERVER_API_H */

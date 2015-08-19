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
 * �����ռ�
 * @param parent ���ռ�
 * @param srv ������
 * @param buckets �ռ��ڹ�ϣ��Ͱ������
 * @return db_space_tʵ��
 */
db_space_t* kdb_space_create(db_space_t* parent, db_server_t* srv, int buckets);

/**
 * ���ٿռ�
 * @param space db_space_tʵ��
 */
void kdb_space_destroy(db_space_t* space);

/**
 * ��ռ�����������, ·���ϲ����ڵĿռ�Ҳ���Զ�����
 * @param space db_space_tʵ��
 * @param path ·��
 * @param full_path ȫ·��
 * @param value ����ֵ
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_set_key(db_space_t* space, const char* path, const char* full_path, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * ��ռ���������
 * @param space db_space_tʵ��
 * @param path ·��
 * @param full_path ȫ·��
 * @param value ����ֵ
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_add_key(db_space_t* space, const char* path, const char* full_path, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * ��ȡ�ռ�����ֵ
 * @param space db_space_tʵ��
 * @param path ·��
 * @param value �ռ�ֵָ�뷵��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_get_key(db_space_t* space, const char* path, db_space_value_t** value);

/**
 * ���ٿռ�����ֵ
 * @param space db_space_tʵ��
 * @param path ·��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_del_key(db_space_t* space, const char* path);

/**
 * ��������ֵ
 * @param space db_space_tʵ��
 * @param path ·��
 * @param value ����ֵ
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_update_key(db_space_t* space, const char* path, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * ʵ��memcache cas(check&set)����
 * @param space db_space_tʵ��
 * @param path ·��
 * @param value ����ֵ
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @param cas_id memcached cas unique
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_cas_key(db_space_t* space, const char* path, const void* value, int size, uint32_t flags, uint32_t exptime, uint64_t cas_id);

/**
 * ���Ŀռ�����
 * @param space db_space_tʵ��
 * @param path ·��
 * @param channel ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_subscribe_key(db_space_t* space, const char* path, kchannel_ref_t* channel);

/**
 * ���Ŀռ䣨�����ӿռ䣩����������
 * @param space db_space_tʵ��
 * @param path ·��
 * @param channel ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_subscribe(db_space_t* space, const char* path, kchannel_ref_t* channel);

/**
 * ȡ�����Ŀռ�����
 * @param space db_space_tʵ��
 * @param path ·��
 * @param channel ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_forget_key(db_space_t* space, const char* path, kchannel_ref_t* channel);

/**
 * ȡ������(�����ӿռ�)
 * @param space db_space_tʵ��
 * @param path ·��
 * @param channel ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_forget(db_space_t* space, const char* path, kchannel_ref_t* channel);

/**
 * ����ӿռ�
 * @param space db_space_tʵ��
 * @param path ·��
 * @param full_path ȫ·��
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_add_space(db_space_t* space, const char* path, const char* full_path, uint32_t exptime);

/**
 * ��ȡ�ӿռ�
 * @param space db_space_tʵ��
 * @param path ·��
 * @param child �ӿռ�ָ�뷵��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_get_space(db_space_t* space, const char* path, db_space_t** child);

/**
 * �����ӿռ�(������������)
 * @param space db_space_tʵ��
 * @param path ·��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_del_space(db_space_t* space, const char* path);

/**
 * ��ȡ������
 * @param space db_space_tʵ��
 * @return db_server_tָ��
 */
db_server_t* kdb_space_get_server(db_space_t* space);

/**
 * ��ȡ�ռ��ڹ�ϣ��Ͱ�ĸ���
 * @param space db_space_tʵ��
 * @return ��ϣ��Ͱ�ĸ���
 */
int kdb_space_get_buckets(db_space_t* space);

/**
 * ��������ֵ
 * @param value ����ֵָ��
 * @param size ����ֵ����
 * @return db_value_tʵ��
 */
db_value_t* value_create(const void* value, int size);

/**
 * ��������ֵ
 * @param value db_value_tʵ��
 */
void value_destroy(db_value_t* value);

/**
 * ȡ��ֵ
 * @param value db_value_tʵ��
 * @return ֵָ��
 */
void* value_get_value(db_value_t* value);

/**
 * ȡ��ֵ����
 * @param value db_value_tʵ��
 * @return ֵ����
 */
int value_get_size(db_value_t* value);

/**
 * ȡ��ֵmemcached flags
 * @param value db_value_tʵ��
 * @return memcached flags
 */
uint32_t value_get_flags(db_value_t* value);

/**
 * ȡ��ֵmemcached cas unique
 * @param value db_value_tʵ��
 * @return memcached cas unique
 */
uint64_t value_get_cas_id(db_value_t* value);

/**
 * ��������
 * @param value db_value_tʵ��
 * @param chanenl ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int value_subscribe(db_value_t* value, kchannel_ref_t* channel);

/**
 * ȡ����������
 * @param value db_space_value_tʵ��
 * @param chanenl ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_value_forget(db_space_value_t* value, kchannel_ref_t* channel);

/**
 * �������Ա仯
 * @param value db_space_value_tʵ��
 * @param type �仯��ʽ
 */
void kdb_space_value_publish(db_space_value_t* value, kdb_sub_type_e type);

/**
 * ȡ������ֵ
 * @param value db_space_value_tʵ��
 * @return db_value_tʵ��
 */
db_value_t* kdb_space_value_get_value(db_space_value_t* value);

/**
 * �����ռ�����ֵ
 * @param owner �����ռ�
 * @param name ֵ����
 * @param value ����ֵָ��
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @return db_space_value_tʵ��
 */
db_space_value_t* kdb_space_value_create_value(db_space_t* owner, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * �����ռ����ӿռ�
 * @param owner �����ռ�
 * @param full_path ȫ·��
 * @param exptime memcached exptime
 * @return db_space_value_tʵ��
 */
db_space_value_t* kdb_space_value_create_space(db_space_t* owner, const char* full_path, const char* name, uint32_t exptime);

/**
 * ���ٿռ�ֵ
 * @param v db_space_value_tʵ��
 */
void kdb_space_value_destroy(db_space_value_t* v);

/**
 * ���ռ�ֵ����(����/�ӿռ�)
 * @param v db_space_value_tʵ��
 * @param type ����
 * @retval 0 ����type����
 * @retval ���� ��type����
 */
int kdb_space_value_check_type(db_space_value_t* v, kdb_space_value_type_e type);

/**
 * ���Ŀռ����������ԣ������ӿռ�
 * @param v db_space_value_tʵ��
 * @param chanenl ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_value_subscribe_space(db_space_value_t* v, kchannel_ref_t* channel);

/**
 * ȡ�����Ŀռ����������ԣ������ӿռ�
 * @param v db_space_value_tʵ��
 * @param chanenl ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_value_forget_space(db_space_value_t* v, kchannel_ref_t* channel);

/**
 * �ռ�ֵ��ϣ�����ٻص�
 */
void value_dtor(void* v);

/**
 * ���Ĺܵ���ϣ�����ٻص�
 */
void sub_dtor(void* v);

/**
 * ·���ַ�����������
 * @param space ��ǰ�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param name_len ��ǰ·������
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_iterate_path(db_space_t* space, const char* path, char* name, int* name_len);

/**
 * ��ռ����������ֵ, ���·���������Զ�����
 * @param space ��ǰ�ռ�
 * @param path ·��
 * @param full_path ȫ·��
 * @param name ��ǰ·��
 * @param value ����ֵָ��
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_set_key_path(db_space_t* space, const char* path, const char* full_path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * ��ռ����������ֵ
 * @param space ��ǰ�ռ�
 * @param path ·��
 * @param full_path ȫ·��
 * @param name ��ǰ·��
 * @param value ����ֵָ��
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_add_key_path(db_space_t* space, const char* path, const char* full_path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * ��ȡ�ռ�������ֵ
 * @param space ��ǰ�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param value �ռ�ֵָ�뷵��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_get_key_path(db_space_t* space, const char* path, const char* name, db_space_value_t** value);

/**
 * ɾ�������ٿռ�������ֵ
 * @param space ��ǰ�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_del_key_path(db_space_t* space, const char* path, const char* name);

/**
 * ���¿ռ�������ֵ
 * @param space ��ǰ�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param value ����ֵָ��
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @param cas_id memcached cas unique
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_update_key_path(db_space_t* space, const char* path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime, uint64_t cas_id);

/**
 * �������ռ�
 * @param space ��ǰ�ռ�
 * @param path ·��
 * @param full_path ȫ·��
 * @param name ��ǰ·��
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_add_space_path(db_space_t* space, const char* path, const char* full_path, const char* name, uint32_t exptime);

/**
 * ��ȡ����ռ�
 * @param space ��ǰ�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param child ����ռ�ָ�뷵��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_get_space_path(db_space_t* space, const char* path, const char* name, db_space_t** child);

/**
 * ��������ռ�
 * @param space ��ǰ�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_del_space_path(db_space_t* space, const char* path, const char* name);

/**
 * ��������ռ�
 * @param space ��ǰ�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param channel ���Ĺܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_subscribe_space_path(db_space_t* space, const char* path, const char* name, kchannel_ref_t* channel);

#endif /* DB_SPACE_H */

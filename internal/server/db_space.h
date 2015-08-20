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

#include "db_internal.h"
#include "db_server_api.h"

/**
 * ���Ŀռ�����
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @param channel ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_subscribe_key(kdb_space_t* space, const char* path, kchannel_ref_t* channel);

/**
 * ���Ŀռ䣨�����ӿռ䣩����������
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @param channel ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_subscribe(kdb_space_t* space, const char* path, kchannel_ref_t* channel);

/**
 * ȡ�����Ŀռ�����
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @param channel ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_forget_key(kdb_space_t* space, const char* path, kchannel_ref_t* channel);

/**
 * ȡ������(�����ӿռ�)
 * @param space kdb_space_tʵ��
 * @param path ·��
 * @param channel ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_forget(kdb_space_t* space, const char* path, kchannel_ref_t* channel);


/**
 * �����ռ�
 * @param parent ���ռ�
 * @param srv ������
 * @param buckets �ռ��ڹ�ϣ��Ͱ������
 * @return kdb_space_tʵ��
 */
kdb_space_t* kdb_space_create(kdb_space_t* parent, kdb_server_t* srv, int buckets);

/**
 * ���ٿռ�
 * @param space kdb_space_tʵ��
 */
void kdb_space_destroy(kdb_space_t* space);

/**
 * ��ȡ������
 * @param space kdb_space_tʵ��
 * @return kdb_server_tָ��
 */
kdb_server_t* kdb_space_get_server(kdb_space_t* space);

/**
 * ��ȡ�ռ��ڹ�ϣ��Ͱ�ĸ���
 * @param space kdb_space_tʵ��
 * @return ��ϣ��Ͱ�ĸ���
 */
int kdb_space_get_buckets(kdb_space_t* space);

/**
 * ��������ֵ
 * @param value ����ֵָ��
 * @param size ����ֵ����
 * @return kdb_value_tʵ��
 */
kdb_value_t* kdb_value_create(const void* value, int size);

/**
 * ��������ֵ
 * @param value kdb_value_tʵ��
 */
void kdb_value_destroy(kdb_value_t* value);

/**
 * ��������
 * @param value kdb_value_tʵ��
 * @param chanenl ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_value_subscribe(kdb_value_t* value, kchannel_ref_t* channel);

/**
 * ȡ����������
 * @param value kdb_space_value_tʵ��
 * @param chanenl ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_value_forget(kdb_space_value_t* value, kchannel_ref_t* channel);

/**
 * �������Ա仯
 * @param value kdb_space_value_tʵ��
 * @param type �仯��ʽ
 */
void kdb_space_value_publish(kdb_space_value_t* value, kdb_sub_type_e type);

/**
 * �����ռ�����ֵ
 * @param owner �����ռ�
 * @param name ֵ����
 * @param value ����ֵָ��
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @return kdb_space_value_tʵ��
 */
kdb_space_value_t* kdb_space_value_create_value(kdb_space_t* owner, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * �����ռ����ӿռ�
 * @param owner �����ռ�
 * @param full_path ȫ·��
 * @param exptime memcached exptime
 * @return kdb_space_value_tʵ��
 */
kdb_space_value_t* kdb_space_value_create_space(kdb_space_t* owner, const char* full_path, const char* name, uint32_t exptime);

/**
 * ���ٿռ�ֵ
 * @param v kdb_space_value_tʵ��
 */
void kdb_space_value_destroy(kdb_space_value_t* v);

/**
 * ���Ŀռ����������ԣ������ӿռ�
 * @param v kdb_space_value_tʵ��
 * @param chanenl ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_value_subscribe_space(kdb_space_value_t* v, kchannel_ref_t* channel);

/**
 * ȡ�����Ŀռ����������ԣ������ӿռ�
 * @param v kdb_space_value_tʵ��
 * @param chanenl ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_value_forget_space(kdb_space_value_t* v, kchannel_ref_t* channel);

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
 * @param path ·��
 * @param name ��ǰ·��
 * @param name_len ��ǰ·������
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_iterate_path(const char* path, char* name, int* name_len);

/**
 * ��ռ����������ֵ, ���·���������Զ�����
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
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
int kdb_space_set_key_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* full_path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * ��ռ����������ֵ
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param value ����ֵָ��
 * @param size ����ֵ����
 * @param flags memcached flags
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_add_key_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime);

/**
 * ��ȡ�ռ�������ֵ
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param value �ռ�ֵָ�뷵��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_get_key_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* name, kdb_space_value_t** value);

/**
 * ɾ�������ٿռ�������ֵ
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_del_key_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* name);

/**
 * ���¿ռ�������ֵ
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
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
int kdb_space_update_key_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime, uint64_t cas_id);

/**
 * �����ռ�������ֵ
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param delta ������ֵ
 * @param value ����ֵָ�뷵��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_incr_decr_key_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* name, uint64_t delta, kdb_space_value_t** value);

/**
 * ��������ֵ
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param channel ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_subscribe_key_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* name, kchannel_ref_t* channel);

/**
 * ȡ����������ֵ
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param channel ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_forget_key_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* name, kchannel_ref_t* channel);

/**
 * ȡ�����Ŀռ�
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param channel ���ĵĹܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_forget_space_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* name, kchannel_ref_t* channel);

/**
 * �������ռ�
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
 * @param path ·��
 * @param full_path ȫ·��
 * @param name ��ǰ·��
 * @param exptime memcached exptime
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_add_space_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* full_path, const char* name, uint32_t exptime);

/**
 * ��ȡ����ռ�
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param child ����ռ�ָ�뷵��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_get_space_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* name, kdb_space_t** child);

/**
 * ��������ռ�
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_del_space_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* name);

/**
 * ��������ռ�
 * @param space ��ǰ�ռ�
 * @param next_space �²�ռ�
 * @param path ·��
 * @param name ��ǰ·��
 * @param channel ���Ĺܵ�
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_space_subscribe_space_path(kdb_space_t* space, kdb_space_t** next_space, const char* path, const char* name, kchannel_ref_t* channel);

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

#ifdef WIN32
/**
 * �ַ���תlong long
 * @param s �ַ���
 * @param size ����
 * @return long long
 */
long long atoll_s(void* s, int size);
#endif /* WIN32 */

/*! �ַ��������� */
#define for_each_char(c, s) \
    for (c = *s++; (c); c = *s++)

#endif /* DB_SPACE_H */

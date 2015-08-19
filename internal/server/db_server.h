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
 * ����������
 * @return kdb_server_tʵ��
 */
kdb_server_t* kdb_server_create();

/**
 * ����
 * @param srv kdb_server_tʵ��
 */
void kdb_server_destroy(kdb_server_t* srv);

/**
 * ����
 * @param srv kdb_server_tʵ��
 * @param argc ��������
 * @param argv ����ָ������
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_server_start(kdb_server_t* srv, int argc, char** argv);

/**
 * ֹͣ
 * @param srv kdb_server_tʵ��
 */
void kdb_server_stop(kdb_server_t* srv);

/**
 * �ȴ�ֹͣ
 * @param srv kdb_server_tʵ��
 */
void kdb_server_wait_for_stop(kdb_server_t* srv);

/**
 * ��ȡ�ռ�(�Ǹ��ռ�)��ϣ��Ͱ����
 * @param srv kdb_server_tʵ��
 * @return �ռ�(�Ǹ��ռ�)��ϣ��Ͱ����
 */
int kdb_server_get_space_buckets(kdb_server_t* srv);

/**
 * ��ȡ�ͻ��˹ܵ���ʱ(��)
 * @param srv kdb_server_tʵ��
 * @return �ͻ��˹ܵ���ʱ(��)
 */
int kdb_server_get_channel_timeout(kdb_server_t* srv);

/**
 * ��ȡ�������ָ��
 * @param srv kdb_server_tʵ��
 * @return �������ָ��
 */
char* kdb_server_get_action_buffer(kdb_server_t* srv);

/**
 * ��ȡ�����������
 * @param srv kdb_server_tʵ��
 * @return �����������
 */
int kdb_server_get_action_buffer_length(kdb_server_t* srv);

/**
 * ���ò���ص� - �¿ռ������ֵ����
 * @param srv kdb_server_tʵ��
 * @param value kdb_space_value_tʵ��
 */
void kdb_server_call_cb_on_add(kdb_server_t* srv, kdb_space_value_t* value);

/**
 * ���ò���ص� - ����ֵɾ��
 * @param srv kdb_server_tʵ��
 * @param value kdb_space_value_tʵ��
 */
void kdb_server_call_cb_on_delete(kdb_server_t* srv, kdb_space_value_t* value);

/**
 * ���ò���ص� - ����ֵ����
 * @param srv kdb_server_tʵ��
 * @param value kdb_space_value_tʵ��
 */
void kdb_server_call_cb_on_update(kdb_server_t* srv, kdb_space_value_t* value);

/**
 * ȡ�ò��ָ��
 * @param srv kdb_server_tʵ��
 * @return kdb_server_plugin_tʵ��
 */
kdb_server_plugin_t* kdb_server_get_plugin(kdb_server_t* srv);

/**
 * ���ز��
 * @param srv kdb_server_tʵ��
 * @param file ����ļ���
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_server_load_plugin(kdb_server_t* srv, const char* file);

/**
 * �����ܵ��ص�
 */
void acceptor_cb(kchannel_ref_t* channel, knet_channel_cb_event_e e);

/**
 * �ͻ��˹ܵ��ص�
 */
void connector_cb(kchannel_ref_t* channel, knet_channel_cb_event_e e);

#endif /* DB_SERVER_H */

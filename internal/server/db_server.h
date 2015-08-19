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

#include "server_config.h"

/**
 * ����������
 * @return db_server_tʵ��
 */
db_server_t* kdb_server_create();

/**
 * ����
 * @param srv db_server_tʵ��
 */
void kdb_server_destroy(db_server_t* srv);

/**
 * ����
 * @param srv db_server_tʵ��
 * @param argc ��������
 * @param argv ����ָ������
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_server_start(db_server_t* srv, int argc, char** argv);

/**
 * ֹͣ
 * @param srv db_server_tʵ��
 */
void kdb_server_stop(db_server_t* srv);

/**
 * �ȴ�ֹͣ
 * @param srv db_server_tʵ��
 */
void kdb_server_wait_for_stop(db_server_t* srv);

/**
 * ��ȡ�ռ�(�Ǹ��ռ�)��ϣ��Ͱ����
 * @param srv db_server_tʵ��
 * @return �ռ�(�Ǹ��ռ�)��ϣ��Ͱ����
 */
int kdb_server_get_space_buckets(db_server_t* srv);

/**
 * ��ȡ�ͻ��˹ܵ���ʱ(��)
 * @param srv db_server_tʵ��
 * @return �ͻ��˹ܵ���ʱ(��)
 */
int kdb_server_get_channel_timeout(db_server_t* srv);

/**
 * ��ȡ�������ָ��
 * @param srv db_server_tʵ��
 * @return �������ָ��
 */
char* kdb_server_get_action_buffer(db_server_t* srv);

/**
 * ��ȡ�����������
 * @param srv db_server_tʵ��
 * @return �����������
 */
int kdb_server_get_action_buffer_length(db_server_t* srv);

/**
 * �����ܵ��ص�
 */
void acceptor_cb(kchannel_ref_t* channel, knet_channel_cb_event_e e);

/**
 * �ͻ��˹ܵ��ص�
 */
void connector_cb(kchannel_ref_t* channel, knet_channel_cb_event_e e);

#endif /* DB_SERVER_H */

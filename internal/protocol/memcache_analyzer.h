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
 * ����memcachedЭ�������
 * @return memcache_analyzer_tʵ��
 */
memcache_analyzer_t* memcache_analyzer_create();

/**
 * ����memcachedЭ�������
 * @param mc memcache_analyzer_tʵ��
 */
void memcache_analyzer_destroy(memcache_analyzer_t* mc);

/**
 * ���ý�����������
 * @param mc memcache_analyzer_tʵ��
 */
void memcache_analyzer_reset(memcache_analyzer_t* mc);

/**
 * ��ȡ������memcachedЭ��
 * @param mc memcache_analyzer_tʵ��
 * @param channel kchannel_ref_tʵ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_drain(memcache_analyzer_t* mc, kchannel_ref_t* channel);

/**
 * ����memcachedЭ��
 * @param mc memcache_analyzer_tʵ��
 * @param channel kchannel_ref_tʵ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_analyze(memcache_analyzer_t* mc, kchannel_ref_t* channel);

/**
 * ���������� - incr, decr
 * @param mc memcache_analyzer_tʵ��
 * @param buffer ������
 * @param pos �����ĵ�ǰλ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_do_analyze_incr_decr(memcache_analyzer_t* mc, const char* buffer, int pos);

/**
 * ���������� - subkey, sub
 * @param mc memcache_analyzer_tʵ��
 * @param buffer ������
 * @param pos �����ĵ�ǰλ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_do_analyze_sub(memcache_analyzer_t* mc, const char* buffer, int pos);

/**
 * ���������� - leave, leavekey
 * @param mc memcache_analyzer_tʵ��
 * @param buffer ������
 * @param pos �����ĵ�ǰλ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_do_analyze_leave(memcache_analyzer_t* mc, const char* buffer, int pos);

/**
 * ���������� - delete, deletespace
 * @param mc memcache_analyzer_tʵ��
 * @param buffer ������
 * @param pos �����ĵ�ǰλ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_do_analyze_delete(memcache_analyzer_t* mc, const char* buffer, int pos);

/**
 * ���������� - get, gets
 * @param mc memcache_analyzer_tʵ��
 * @param buffer ������
 * @param pos �����ĵ�ǰλ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_do_analyze_get(memcache_analyzer_t* mc, const char* buffer, int pos, int gets);

/**
 * ���������� - �洢������
 * @param mc memcache_analyzer_tʵ��
 * @param buffer ������
 * @param pos �����ĵ�ǰλ��
 * @param cas �Ƿ���cas����
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_do_analyze_store(memcache_analyzer_t* mc, const char* buffer, int pos, int cas);

/**
 * ���������� - addspace
 * @param mc memcache_analyzer_tʵ��
 * @param buffer ������
 * @param pos �����ĵ�ǰλ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_do_analyze_addspace(memcache_analyzer_t* mc, const char* buffer, int pos);

/**
 * ����������
 * @param mc memcache_analyzer_tʵ��
 * @param buffer ������
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_analyze_command_line(memcache_analyzer_t* mc, const char* buffer);

/**
 * ���������� - ��ȡ����
 * @param mc memcache_analyzer_tʵ��
 * @param command ������
 * @param size command����
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_command_line_get(const char* s, char* command, int size);

/**
 * ִ������
 * @param mc memcache_analyzer_tʵ��
 * @param channel kchannel_ref_tʵ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int memcache_analyzer_do_command(memcache_analyzer_t* mc, kchannel_ref_t* channel);

/**
 * ���������
 * @param mc memcache_analyzer_tʵ��
 * @param channel kchannel_ref_tʵ��
 * @param error ������
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
void memcache_analyzer_return(memcache_analyzer_t* mc, kchannel_ref_t* channel, int error);

/**
 * ���������¼�
 * @param channel kchannel_ref_tʵ��
 * @param path ·��
 * @param dv db_space_value_tʵ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int publish_update(kchannel_ref_t* channel, const char* path, db_space_value_t* dv);

/**
 * ���������¼�
 * @param channel kchannel_ref_tʵ��
 * @param path ·��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int publish_delete(kchannel_ref_t* channel, const char* path);

#endif /* MEMCACHE_ANALYZER_H */

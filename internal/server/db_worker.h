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

#ifndef DB_WORKER_H
#define DB_WORKER_H

#include "db_internal.h"

/**
 * ���������߳�
 * @return kdb_worker_tʵ��
 */
kdb_worker_t* kdb_worker_create();

/**
 * ���ٹ����߳�
 * @param worker kdb_worker_tʵ��
 */
void kdb_worker_destroy(kdb_worker_t* worker);

/**
 * ���������߳�
 * @param worker kdb_worker_tʵ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_worker_start(kdb_worker_t* worker);

/**
 * �رչ����߳�
 * @param worker kdb_worker_tʵ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_worker_stop(kdb_worker_t* worker);

/**
 * �ȴ������߳̽���
 * @param worker kdb_worker_tʵ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_worker_wait_for_stop(kdb_worker_t* worker);

/**
 * Ͷ������
 * @param worker kdb_worker_tʵ��
 * @param ptr ����ָ��
 * @retval db_error_ok �ɹ�
 * @retval ���� ʧ��
 */
int kdb_worker_push(kdb_worker_t* worker, void* ptr);

#endif /* DB_WORKER_H */

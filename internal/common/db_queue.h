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

#ifndef DB_QUEUE_H
#define DB_QUEUE_H

typedef struct _queue_t kdb_queue_t;

/**
 * ��������
 * @param size ������󳤶�
 * @return kdb_queue_tʵ��
 */
kdb_queue_t* kdb_queue_create(int size);

/**
 * ���ٶ���
 * @param kq kdb_queue_tʵ��
 */
void kdb_queue_destroy(kdb_queue_t* kq);

/**
 * ��ָ��������β
 * @param kq kdb_queue_tʵ��
 * @param ptr ָ��
 * @retval 0 �ɹ�
 * @retval ���� ʧ��
 */
int kdb_queue_push(kdb_queue_t* kq, void* ptr);

/**
 * �Ӷ���ͷȡ��һ��ָ��
 * @param kq kdb_queue_tʵ��
 * @retval 0 ����Ϊ��
 * @retval ���� ָ��
 */
void* kdb_queue_pop(kdb_queue_t* kq);

/**
 * �Ӷ���ͷȡ��һ��ָ��, �����Ӷ�����ɾ��
 * @param kq kdb_queue_tʵ��
 * @retval 0 ����Ϊ��
 * @retval ���� ָ��
 */
void* kdb_queue_peek(kdb_queue_t* kq);

/**
 * ��ȡ������Ԫ�ظ���
 * @param kq kdb_queue_tʵ��
 * @return Ԫ�ظ���
 */
int kdb_queue_get_count(kdb_queue_t* kq);

#endif /* DB_QUEUE_H */

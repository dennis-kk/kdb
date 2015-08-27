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
 * 建立队列
 * @param size 队列最大长度
 * @return kdb_queue_t实例
 */
kdb_queue_t* kdb_queue_create(int size);

/**
 * 销毁队列
 * @param kq kdb_queue_t实例
 */
void kdb_queue_destroy(kdb_queue_t* kq);

/**
 * 将指针放入队列尾
 * @param kq kdb_queue_t实例
 * @param ptr 指针
 * @retval 0 成功
 * @retval 其他 失败
 */
int kdb_queue_push(kdb_queue_t* kq, void* ptr);

/**
 * 从队列头取出一个指针
 * @param kq kdb_queue_t实例
 * @retval 0 队列为空
 * @retval 非零 指针
 */
void* kdb_queue_pop(kdb_queue_t* kq);

/**
 * 从队列头取出一个指针, 但不从队列内删除
 * @param kq kdb_queue_t实例
 * @retval 0 队列为空
 * @retval 非零 指针
 */
void* kdb_queue_peek(kdb_queue_t* kq);

/**
 * 获取队列内元素个数
 * @param kq kdb_queue_t实例
 * @return 元素个数
 */
int kdb_queue_get_count(kdb_queue_t* kq);

#endif /* DB_QUEUE_H */

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

#ifndef DB_COMMON_H
#define DB_COMMON_H

/**
 * ��������
 */
typedef enum _command_type_e {
    command_type_set = 1,     /* ����ֵ */
    command_type_add,         /* ���ֵ */
    command_type_addspace,    /* �½��ռ� */
    command_type_replace,     /* �滻 */
    command_type_append,      /* ǰ��� - δʵ�� */
    command_type_prepend,     /* ����� - δʵ�� */
    command_type_cas,         /* Check&Swap */
    command_type_get,         /* ��ȡ��ֵ */ 
    command_type_gets,        /* ��ȡ��ֵ */
    command_type_delete,      /* ����ֵ */
    command_type_deletespace, /* ���ٿռ� */
    command_type_incr,        /* incr���� */
    command_type_decr,        /* incr�ݼ� */
    command_type_subkey,      /* ����ֵ */
    command_type_sub,         /* ���Ŀռ� */
    command_type_leavekey,    /* ȡ������ - ֵ */
    command_type_leave,       /* ȡ������ - �ռ� */
    command_type_quit,        /* �˳� */
} command_type_e;

#define MAX_PATH_SIZE              MAX_NAME_SIZE * 4  /* ���·������ */
#define COMMAND_LINE_LENGTH        1024               /* ���������е���󳤶�(������ֵ) */
#define COMMAND_LENGTH             32                 /* ������󳤶� */
#define FLAGS_LENGTH               32                 /* memcached flags�ֶ���󳤶� */
#define EXPTIME_LENGTH             32                 /* memcached exptime�ֶ���󳤶� */
#define BYTES_LENGTH               32                 /* memcached bytes�ֶ���󳤶� */
#define NOREPLY_LENGTH             16                 /* memcached noreply�ֶ���󳤶� */
#define CAS_UNIQUE_LENGTH          64                 /* memcached CAS unique ID�ֶ���󳤶� */
#define MAX_KEYS                   128                /* gets������������ */
#define CHANGE_VALUE_LENGTH        64                 /* memcached incr, decrֵ�ֶ���󳤶� */
#define CRLF                       "\r\n"
#define NOREPLY                    "noreply"
#define WHITE_SPACE                ' '
#define CR                         '\r'
#define SET                        "set"
#define SUBKEY                     "subkey"
#define SUB                        "sub"
#define ADD                        "add"
#define ADDSPACE                   "addspace"
#define APPEND                     "append"
#define LEAVEKEY                   "leavekey"
#define LEAVE                      "leave"
#define REPLACE                    "replace"
#define PREPEND                    "prepend"
#define CAS                        "cas"
#define GET                        "get"
#define GETS                       "gets"
#define KDB_DELETE                 "delete"
#define DELETESPACE                "deletespace"
#define DECR                       "decr"
#define INCR                       "incr"
#define QUIT                       "quit"

/*! δʹ��*/
#define UNUSED(v) (void)(v)
/*! �ַ��������� */
#define for_each_char(c, s) for (c = *s++; (c); c = *s++)

#endif /* DB_COMMON_H */

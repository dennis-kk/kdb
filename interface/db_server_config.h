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

#ifndef DB_SERVER_CONFIG_H
#define DB_SERVER_CONFIG_H

typedef enum _db_error_e {
    db_error_ok = 0,                    /* �ɹ� */
    db_error_invalid_path,              /* ·����Ч */
    db_error_exist,                     /* �ڵ��Ѿ����� */
    db_error_invalid_type,              /* λ�õĽڵ����� */
    db_error_invalid_format,            /* ·����ʽ����������ʽ���� */
    db_error_key_name_overflow,         /* �ڵ�����ֳ��ȳ��������� */
    db_error_hash_insert,               /* �ռ��ϣ�����ʧ�� */
    db_error_hash_delete,               /* �ռ��ϣ��ɾ��ʧ�� */
    db_error_path_need_space_but_value, /* ·����ǰ��Ҫһ���ռ�ڵ㣬����ǰΪ���Խڵ� */
    db_error_path_not_found,            /* ·�������� */
    db_error_sub_fail,                  /* ����ʧ�� */
    db_error_server_start_thread_fail,  /* ���������������߳�ʧ�� */
    db_error_channel_need_more,         /* ��Ҫ�����������ִ������ */
    db_error_unknown_command,           /* δ֪���� */
    db_error_command_not_impl,          /* ����δʵ�� */
    db_error_cas_fail,                  /* CAS����ʧ�� */
    db_error_listen_fail,               /* �������������ʧ�� */
    db_error_load_plugin,               /* ���ز��ʧ�� */
} db_error_e;

typedef struct _db_server_t      kdb_server_t;
typedef struct _db_space_t       kdb_space_t;
typedef struct _db_space_value_t kdb_space_value_t;
typedef struct _db_value_t       kdb_value_t;

/**
 * �ռ�ֵ����
 */
typedef enum _db_space_value_type_e {
    space_value_type_value = 1, /* ���� */
    space_value_type_space = 2, /* �ռ� */
} kdb_space_value_type_e;

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#   if defined(__cplusplus) && defined(_MSC_VER)
#	    define PluginFunc extern "C" __declspec(dllexport)
#   elif defined(__cplusplus)
#	    define PluginFunc extern "C"
#   else
#	    define PluginFunc
#   endif /* __cplusplus */
#else
#	include <dlfcn.h>
#   if defined(__cplusplus)
#	    define PluginFunc extern "C"
#   else
#	    define PluginFunc
#   endif /* __cplusplus */
#endif /* WIN32 */

/*! �ص�����ԭ�� - ������������ɺ���� */
typedef int (*kdb_server_on_after_start_t)(kdb_server_t*);
/*! �ص�����ԭ�� - �������ر���ɺ����(����δ�����������Է���) */
typedef int (*kdb_server_on_after_stop_t)(kdb_server_t*);
/*! �ص�����ԭ�� - ���µĿռ�����Խ����� */
typedef int (*kdb_server_on_key_after_add_t)(kdb_server_t*, kdb_space_value_t*);
/*! �ص�����ԭ�� - ���Ա����ĺ� */
typedef int (*kdb_server_on_key_after_update_t)(kdb_server_t*, kdb_space_value_t*);
/*! �ص�����ԭ�� - ���Ա�����ǰ */
typedef int (*kdb_server_on_key_before_delete_t)(kdb_server_t*, kdb_space_value_t*);

#endif /* DB_SERVER_CONFIG_H */

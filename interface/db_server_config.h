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
    db_error_ok = 0,                    /* 成功 */
    db_error_invalid_path,              /* 路径无效 */
    db_error_exist,                     /* 节点已经存在 */
    db_error_invalid_type,              /* 位置的节点类型 */
    db_error_invalid_format,            /* 路径格式错误或命令格式错误 */
    db_error_key_name_overflow,         /* 节点的名字长度超出了限制 */
    db_error_hash_insert,               /* 空间哈希表插入失败 */
    db_error_hash_delete,               /* 空间哈希表删除失败 */
    db_error_path_need_space_but_value, /* 路径当前需要一个空间节点，但当前为属性节点 */
    db_error_path_not_found,            /* 路径不存在 */
    db_error_sub_fail,                  /* 订阅失败 */
    db_error_server_start_thread_fail,  /* 启动服务器工作线程失败 */
    db_error_channel_need_more,         /* 需要更多的数据来执行命令 */
    db_error_unknown_command,           /* 未知命令 */
    db_error_command_not_impl,          /* 命令未实现 */
    db_error_cas_fail,                  /* CAS操作失败 */
    db_error_listen_fail,               /* 开启网络监听器失败 */
    db_error_load_plugin,               /* 加载插件失败 */
} db_error_e;

typedef struct _db_server_t      kdb_server_t;
typedef struct _db_space_t       kdb_space_t;
typedef struct _db_space_value_t kdb_space_value_t;
typedef struct _db_value_t       kdb_value_t;

/**
 * 空间值类型
 */
typedef enum _db_space_value_type_e {
    space_value_type_value = 1, /* 属性 */
    space_value_type_space = 2, /* 空间 */
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

/*! 回调函数原型 - 服务器启动完成后调用 */
typedef int (*kdb_server_on_after_start_t)(kdb_server_t*);
/*! 回调函数原型 - 服务器关闭完成后调用(数据未被清理，还可以访问) */
typedef int (*kdb_server_on_after_stop_t)(kdb_server_t*);
/*! 回调函数原型 - 有新的空间或属性建立后 */
typedef int (*kdb_server_on_key_after_add_t)(kdb_server_t*, kdb_space_value_t*);
/*! 回调函数原型 - 属性被更改后 */
typedef int (*kdb_server_on_key_after_update_t)(kdb_server_t*, kdb_space_value_t*);
/*! 回调函数原型 - 属性被销毁前 */
typedef int (*kdb_server_on_key_before_delete_t)(kdb_server_t*, kdb_space_value_t*);

#endif /* DB_SERVER_CONFIG_H */

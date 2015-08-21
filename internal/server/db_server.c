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

#include "db_server.h"
#include "db_space.h"
#include "memcache_analyzer.h"

kdb_server_t*   db_server   = 0;
kloop_t*        server_loop = 0;
ktimer_loop_t*  timer_loop  = 0;
kdb_space_t*    root_space  = 0;

struct _server_plugin_t {
    kdb_server_on_after_start_t       start_cb;
    kdb_server_on_after_stop_t        stop_cb;
    kdb_server_on_key_after_add_t     add_cb;
    kdb_server_on_key_after_update_t  update_cb;
    kdb_server_on_key_before_delete_t delete_cb;
};

struct _db_server_t {
    kthread_runner_t*   runner;
    kdb_server_plugin_t plugin;
    char                plugin_path[PLUGIN_MAX_PATH];
    char                ip[32];
    int                 port;
    int                 root_space_buckets;
    int                 space_buckets;
    int                 channel_timeout;
    char*               action_buffer;
    int                 timer_freq;
    int                 timer_slot;
#   ifdef WIN32
    HMODULE             plugin_handle;
#   else
    void*               plugin_handle;
#   endif /* WIN32 */
};

kdb_server_t* kdb_server_create() {
    kdb_server_t* srv = create(kdb_server_t);
    memset(srv, 0, sizeof(kdb_server_t));
    assert(srv);
    srv->action_buffer = create_raw(ACTION_BUFFER_LENGTH);
    assert(srv->action_buffer);
    return srv;
}

void kdb_server_destroy(kdb_server_t* srv) {
    assert(srv);
    if (server_loop) {
        knet_loop_destroy(server_loop);
    }
    if (timer_loop) {
        ktimer_loop_destroy(timer_loop);
    }
    if (root_space) {
        kdb_space_destroy(root_space);
    }
    if (srv->action_buffer) {
        destroy(srv->action_buffer);
    }
    if (srv->plugin_handle) {
#       ifdef WIN32
        FreeLibrary(srv->plugin_handle);
#       else
        dlclose(srv->plugin_handle);
#       endif /* WIN32 */
    }
    destroy(srv);
}

void kdb_server_parse_command_line(kdb_server_t* srv, int argc, char** argv) {
    int i = 1;
    /* 默认值 */
    srv->root_space_buckets = ROOT_SPACE_DEFAULT_BUCKETS;
    srv->space_buckets      = SPACE_DEFAULT_BUCKETS;
    srv->port               = SERVER_DEFAULT_PORT;
    srv->channel_timeout    = CHANNEL_DEFAULT_TIMEOUT;
    srv->timer_freq         = TIMER_DEFAULT_FREQ;
    srv->timer_slot         = TIMER_DEFAULT_SLOT;
    strcpy(srv->ip, SERVER_DEFAULT_IP);
    /* 命令行参数 */
    if (argc < 2) {
        return;
    }
    for (; i < argc;) {
        if (EQUAL(argv[i], "-ip")) {
            if (i + 1 < argc) {
                strcpy(srv->ip, argv[++i]);
            }
        } else if (EQUAL(argv[i], "-port")) {
            if (i + 1 < argc) {
                srv->port = atoi(argv[++i]);
            }
        } else if (EQUAL(argv[i], "-channel-timeout")) {
            if (i + 1 < argc) {
                srv->channel_timeout = atoi(argv[++i]);
            }
        } else if (EQUAL(argv[i], "-root-buckets")) {
            if (i + 1 < argc) {
                srv->root_space_buckets = atoi(argv[++i]);
            }
        } else if (EQUAL(argv[i], "-space-buckets")) {
            if (i + 1 < argc) {
                srv->space_buckets = atoi(argv[++i]);
            }
        } else if (EQUAL(argv[i], "-timer-freq")) {
            if (i + 1 < argc) {
                srv->timer_freq = atoi(argv[++i]);
            }
        } else if (EQUAL(argv[i], "-timer-slot")) {
            if (i + 1 < argc) {
                srv->timer_slot = atoi(argv[++i]);
            }
        } else if (EQUAL(argv[i], "-plugin")) {
            if (i + 1 < argc) {
                strcpy(srv->plugin_path, argv[++i]);
            }
        } else {
            i += 1;
        }
    }
}

int kdb_server_start(kdb_server_t* srv, int argc, char** argv) {
    kchannel_ref_t* acceptor = 0;
    assert(srv);
    /* 命令行参数 */
    kdb_server_parse_command_line(srv, argc, argv);
    /* 建立根空间 */
    root_space = kdb_space_create(0, srv, srv->root_space_buckets);
    assert(root_space);
    /* 建立网络循环 */
    server_loop = knet_loop_create();
    assert(server_loop);
    /* 建立监听器 */
    acceptor = knet_loop_create_channel(server_loop, 128, ACTION_BUFFER_LENGTH);
    assert(acceptor);
    knet_channel_ref_set_cb(acceptor, acceptor_cb);
    printf("Starting @ %s:%d...\n", srv->ip, srv->port);
    if (error_ok != knet_channel_ref_accept(acceptor, srv->ip, srv->port, 1024)) {
        return db_error_listen_fail;
    }
    /* 建立定时器循环 */
    timer_loop = ktimer_loop_create(srv->timer_freq, srv->timer_slot);
    assert(timer_loop);
    /* 建立工作线程 */
    srv->runner = thread_runner_create(0, 0);
    assert(srv->runner);
    /* 启动工作线程 */
    if (error_ok != thread_runner_start_multi_loop_varg(srv->runner, 0, "lt", server_loop, timer_loop)) {
        return db_error_server_start_thread_fail;
    }
    if (srv->plugin_path[0]) {
        kdb_server_load_plugin(srv, srv->plugin_path);
    } else {
#       ifdef WIN32
        kdb_server_load_plugin(srv, "db_plugin.dll");
#       else
        kdb_server_load_plugin(srv, "db_plugin.so");
#       endif /* WIN32 */
    }
    if (srv->plugin.start_cb) {
        srv->plugin.start_cb(srv);
    }
    return db_error_ok;
}

void kdb_server_stop(kdb_server_t* srv) {
    assert(srv);
    if (srv->runner) {
        thread_runner_stop(srv->runner);
    }
}

void kdb_server_wait_for_stop(kdb_server_t* srv) {
    assert(srv);
    if (srv->runner) {
        thread_runner_join(srv->runner);
    }
    if (srv->plugin.stop_cb) {
        srv->plugin.stop_cb(srv);
    }
}

int kdb_server_get_space_buckets(kdb_server_t* srv) {
    assert(srv);
    return srv->space_buckets;
}

int kdb_server_get_channel_timeout(kdb_server_t* srv) {
    assert(srv);
    return srv->channel_timeout;
}

char* kdb_server_get_action_buffer(kdb_server_t* srv) {
    assert(srv);
    return srv->action_buffer;
}

int kdb_server_get_action_buffer_length(kdb_server_t* srv) {
    (void)srv;
    return ACTION_BUFFER_LENGTH;
}

kdb_space_t* kdb_server_get_root_space(kdb_server_t* srv) {
    (void)srv;
    return root_space;
}

void acceptor_cb(kchannel_ref_t* channel, knet_channel_cb_event_e e) {
    memcache_analyzer_t* mc = 0;
    if (e & channel_cb_event_accept) {
        /* 设置超时 */
        knet_channel_ref_set_timeout(channel, kdb_server_get_channel_timeout(db_server));
        /* 设置回调 */
        knet_channel_ref_set_cb(channel, connector_cb);
        /* 建立memcached兼容的协议解析器 */
        mc = memcache_analyzer_create();
        assert(mc);
        knet_channel_ref_set_ptr(channel, mc);
    }
}

void connector_cb(kchannel_ref_t* channel, knet_channel_cb_event_e e) {
    memcache_analyzer_t* mc = (memcache_analyzer_t*)knet_channel_ref_get_ptr(channel);
    if (e & channel_cb_event_recv) {
        if (!mc) {
            knet_channel_ref_close(channel);
            return;
        }
        /* 处理 */
        memcache_analyzer_drain(mc, channel);
    } else if (e & channel_cb_event_close) {
        if (mc) {
            memcache_analyzer_destroy(mc);
        }
        knet_channel_ref_set_ptr(channel, 0);
    }
}

kdb_server_plugin_t* kdb_server_get_plugin(kdb_server_t* srv) {
    assert(srv);
    return &srv->plugin;
}

void kdb_server_call_cb_on_add(kdb_server_t* srv, kdb_space_value_t* value) {
    assert(srv);
    assert(value);
    if (srv->plugin.add_cb) {
        srv->plugin.add_cb(srv, value);
    }
}

void kdb_server_call_cb_on_delete(kdb_server_t* srv, kdb_space_value_t* value) {
    assert(srv);
    assert(value);
    if (srv->plugin.delete_cb) {
        srv->plugin.delete_cb(srv, value);
    }
}

void kdb_server_call_cb_on_update(kdb_server_t* srv, kdb_space_value_t* value) {
    assert(srv);
    assert(value);
    if (srv->plugin.update_cb) {
        srv->plugin.update_cb(srv, value);
    }
}

const char* get_exe_path() {
    const static int SIZE      = 512;
    static char      path[512] = {0};
    int              result    = 0;
    int              i         = 0;
#ifdef WIN32    
    static char  seperator = '\\';
    GetModuleFileNameA(NULL, path, sizeof(path));
    result = strlen(path);
#else
    static char  seperator = '/';
    result = readlink("/proc/self/exe", path, sizeof(path));
    if (result < 0 || (result >= SIZE - 1)) {
        return 0;
    }
#endif // WIN32
    path[result] = 0;
    for (i = result; i >= 0; i--) {
        if (path[i] == seperator) {
            path[i] = 0;
            break;
        }
    }
    return path;
}

int kdb_server_load_plugin(kdb_server_t* srv, const char* file) {
#	ifdef WIN32
    SetDllDirectoryA(get_exe_path());
    srv->plugin_handle = LoadLibraryA(file);
    if (!srv->plugin_handle) {
        return db_error_load_plugin;
    }
    srv->plugin.add_cb    = (kdb_server_on_key_after_add_t)GetProcAddress(srv->plugin_handle, "on_add");
    srv->plugin.delete_cb = (kdb_server_on_key_before_delete_t)GetProcAddress(srv->plugin_handle, "on_delete");
    srv->plugin.start_cb  = (kdb_server_on_after_start_t)GetProcAddress(srv->plugin_handle, "on_start");
    srv->plugin.stop_cb   = (kdb_server_on_after_stop_t)GetProcAddress(srv->plugin_handle, "on_stop");
    srv->plugin.update_cb = (kdb_server_on_key_after_update_t)GetProcAddress(srv->plugin_handle, "on_update");
#	else
	srv->plugin_handle = dlopen(file, RTLD_NOW);
    if (!srv->plugin_handle) {
        return db_error_load_plugin;
    }
    srv->plugin.add_cb    = (kdb_server_on_key_after_add_t)dlsym(srv->plugin_handle, "on_add");
    srv->plugin.delete_cb = (kdb_server_on_key_before_delete_t)dlsym(srv->plugin_handle, "on_delete");
    srv->plugin.start_cb  = (kdb_server_on_after_start_t)dlsym(srv->plugin_handle, "on_start");
    srv->plugin.stop_cb   = (kdb_server_on_after_stop_t)dlsym(srv->plugin_handle, "on_stop");
    srv->plugin.update_cb = (kdb_server_on_key_after_update_t)dlsym(srv->plugin_handle, "on_update");
#	endif // WIN32
    return db_error_ok;
}

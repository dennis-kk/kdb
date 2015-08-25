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

#include "memcache_analyzer.h"
#include "db_server.h"
#include "db_space.h"

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

/**
 * memcacheЭ��ʵ��
 */
struct _memcache_analyzer_t {
    char               command_line[COMMAND_LINE_LENGTH];      /* ������ */
    char               command[COMMAND_LENGTH];                /* ���� */
    char               key[MAX_PATH_SIZE];                     /* �� */
    char               keys[MAX_KEYS][MAX_PATH_SIZE];          /* ��� */
    char               data[ACTION_BUFFER_LENGTH];             /* ֵ */
    char               flags_char[FLAGS_LENGTH];               /* memcached flags */
    uint32_t           flags;                                  /* memcached flags */
    char               exptime_char[EXPTIME_LENGTH];           /* memcached exptime(����ʱ�䣬��), ��֧��unixʱ�� */
    uint32_t           exptime;                                /* memcached exptime(����ʱ�䣬��) */
    int                recv_bytes;                             /* ��ǰֵ�Ѿ����յ����ֽ� */
    char               bytes_char[BYTES_LENGTH];               /* ֵ���� */
    int                bytes;                                  /* ֵ���� */
    char               noreply_char[NOREPLY_LENGTH];           /* memcached noreply */
    int                noreply;                                /* memcached noreply */
    char               cas_unique_char[CAS_UNIQUE_LENGTH];     /* memcached CAS unique ID */
    uint64_t           cas_unique;                             /* memcached CAS unique ID */
    int                command_type;                           /* ��ǰ�������� */
    int                key_count;                              /* gets�������� */
    char               change_value_char[CHANGE_VALUE_LENGTH]; /* incr, decrֵ*/
    uint64_t           change_value;                           /* incr, decrֵ*/
    kdb_space_value_t* return_value;                           /* get���ص�ֵ*/
    kdb_space_value_t* return_value_array[MAX_KEYS];           /* gets���ص�ֵ����*/
};

memcache_analyzer_t* memcache_analyzer_create() {
    memcache_analyzer_t* mc = create(memcache_analyzer_t);
    assert(mc);
    memcache_analyzer_reset(mc);
    return mc;
}

void memcache_analyzer_destroy(memcache_analyzer_t* mc) {
    assert(mc);
    destroy(mc);
}

int memcache_analyzer_drain(memcache_analyzer_t* mc, kchannel_ref_t* channel) {
    int error = db_error_ok;
    assert(mc);
    assert(channel);
    /* ����Э�鲢���� */
    while (db_error_ok == error) {
        error = memcache_analyzer_analyze(mc, channel);
    }
    return error;
}

int memcache_analyzer_command_line_get(const char* s, char* command, int size) {
    int pos = 0;
    int i   = 0;
    for (; *s; s++, pos++) {
        if (i && (*s == CR)) { /* ����һ�����ݷָ� */
            break;
        }
        if (*s == WHITE_SPACE) { /* ��������ָ� */
            if (i) {
                break;
            } else {
                continue; /* trim��ߵĿո� */
            }
        }        
        command[i++] = *s;
        if (i >= size) { /* ��� */
            return 0;
        }
    }
    /* ���� */
    command[i] = 0;
    return pos;
}

/*
 * incr <key> <value> [noreply]\r\n
 * decr <key> <value> [noreply]\r\n
*/
int memcache_analyzer_do_analyze_incr_decr(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int bytes = 0;
    /* key */
    GET_FORWARD(buffer, mc->key, MAX_PATH_SIZE - 1, pos, bytes, db_error_invalid_format);
    /* change value */
    GET_FORWARD(buffer, mc->change_value_char, CHANGE_VALUE_LENGTH - 1, pos, bytes, db_error_invalid_format);
    mc->change_value = atoll(mc->change_value_char);
    /* noreply */
    EQUAL_RETURN(mc->noreply_char, MEMCACHED_CRLF, db_error_ok); /* \r\n? */
    NOT_EQUAL_RETURN(mc->noreply_char, MEMCACHED_NOREPLY, db_error_invalid_format); /* noreply? */
    mc->noreply = 1;
    /* \r\n */
    GET_HOLD(buffer, mc->noreply_char, NOREPLY_LENGTH - 1, pos, bytes, db_error_invalid_format);
    NOT_EQUAL_RETURN(mc->noreply_char, MEMCACHED_CRLF, db_error_invalid_format); /* \r\n? */
    return db_error_ok;
}

/*
 * subkey <key> [noreply]\r\n
 * sub <key> [noreply]\r\n
 */
int memcache_analyzer_do_analyze_sub(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int bytes = 0;
    /* key */
    GET_FORWARD(buffer, mc->key, MAX_PATH_SIZE - 1,  pos, bytes, db_error_invalid_format);
    /* noreply */
    GET_FORWARD(buffer, mc->noreply_char, NOREPLY_LENGTH - 1, pos, bytes, db_error_invalid_format);
    EQUAL_RETURN(mc->noreply_char, MEMCACHED_CRLF, db_error_ok); /* \r\n? */
    NOT_EQUAL_RETURN(mc->noreply_char, MEMCACHED_NOREPLY, db_error_invalid_format); /* noreply? */
    mc->noreply = 1;
    /* \r\n */
    GET_HOLD(buffer, mc->noreply_char, NOREPLY_LENGTH - 1, pos, bytes, db_error_invalid_format);
    NOT_EQUAL_RETURN(mc->noreply_char, MEMCACHED_CRLF, db_error_invalid_format);
    return db_error_ok;
}

/*
 * leavekey <key> [noreply]\r\n
 * leave <key> [noreply]\r\n
 */
int memcache_analyzer_do_analyze_leave(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int bytes = 0;
    /* key */
    GET_FORWARD(buffer, mc->key, MAX_PATH_SIZE - 1,  pos, bytes, db_error_invalid_format);
    /* noreply */
    GET_FORWARD(buffer, mc->noreply_char, NOREPLY_LENGTH - 1, pos, bytes, db_error_invalid_format);
    EQUAL_RETURN(mc->noreply_char, MEMCACHED_CRLF, db_error_ok); /* \r\n? */
    NOT_EQUAL_RETURN(mc->noreply_char, MEMCACHED_NOREPLY, db_error_invalid_format); /* noreply? */
    mc->noreply = 1;
    /* \r\n */
    GET_FORWARD(buffer, mc->noreply_char, NOREPLY_LENGTH - 1, pos, bytes, db_error_invalid_format);
    NOT_EQUAL_RETURN(mc->noreply_char, MEMCACHED_CRLF, db_error_invalid_format);
    return db_error_ok;
}

/*
 * delete <key> [noreply]\r\n
 */
int memcache_analyzer_do_analyze_delete(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int bytes = 0;
    /* key */
    GET_FORWARD(buffer, mc->key, MAX_PATH_SIZE - 1,  pos, bytes, db_error_invalid_format);
    /* noreply */
    GET_FORWARD(buffer, mc->noreply_char, NOREPLY_LENGTH - 1, pos, bytes, db_error_invalid_format);
    EQUAL_RETURN(mc->noreply_char, MEMCACHED_CRLF, db_error_ok); /* \r\n */
    NOT_EQUAL_RETURN(mc->noreply_char, MEMCACHED_NOREPLY, db_error_invalid_format); /* noreply? */
    mc->noreply = 1;
    /* \r\n */
    GET_HOLD(buffer, mc->noreply_char, NOREPLY_LENGTH - 1, pos, bytes, db_error_invalid_format);
    NOT_EQUAL_RETURN(mc->noreply_char, MEMCACHED_CRLF, db_error_invalid_format);
    return db_error_ok;
}

/*
 * get <key>\r\n
 * gets <key>*\r\n
 */
int memcache_analyzer_do_analyze_get(memcache_analyzer_t* mc, const char* buffer, int pos, int gets) {
    int bytes = 0;
    if (!gets) { /* get */
        GET_HOLD(buffer, mc->key, MAX_PATH_SIZE - 1, pos, bytes, db_error_invalid_format);
    } else { /* gets */
        for (;;) {
            /* key */
            GET_FORWARD(buffer, mc->keys[mc->key_count], MAX_PATH_SIZE - 1, pos, bytes, db_error_ok);
            EQUAL_RETURN(mc->keys[mc->key_count], MEMCACHED_CRLF, db_error_ok);
            mc->key_count += 1;
        }
    }
    return db_error_ok;
}

/*
 * <command name> <key> <flags> <exptime> <bytes> [noreply]\r\n
 * cas <key> <flags> <exptime> <bytes> <cas unique> [noreply]\r\n
 */

int memcache_analyzer_do_analyze_store(memcache_analyzer_t* mc, const char* buffer, int pos, int cas) {
    int bytes = 0;
    /* key */
    GET_FORWARD(buffer, mc->key, MAX_PATH_SIZE - 1, pos, bytes, db_error_invalid_format);
    /* flags */
    GET_FORWARD(buffer, mc->flags_char, FLAGS_LENGTH - 1,  pos, bytes, db_error_invalid_format);
    mc->flags = atol(mc->flags_char);
    /* exptime */
    GET_FORWARD(buffer, mc->exptime_char, EXPTIME_LENGTH - 1, pos, bytes, db_error_invalid_format);
    mc->exptime = atol(mc->exptime_char);
    /* bytes */
    GET_FORWARD(buffer, mc->bytes_char, BYTES_LENGTH - 1, pos, bytes, db_error_invalid_format);
    mc->bytes = atol(mc->bytes_char) + 2; /* ����\r\n */
    if (cas) {
        /* CAS unique ID */
        GET_FORWARD(buffer, mc->cas_unique_char, CAS_UNIQUE_LENGTH - 1, pos, bytes, db_error_invalid_format);
        mc->cas_unique = atoll(mc->cas_unique_char);
    }
    /* noreply */
    GET_FORWARD(buffer, mc->noreply_char, NOREPLY_LENGTH - 1, pos, bytes, db_error_invalid_format);
    EQUAL_RETURN(mc->noreply_char, MEMCACHED_CRLF, db_error_ok); /* \r\n?*/
    NOT_EQUAL_RETURN(mc->noreply_char, MEMCACHED_NOREPLY, db_error_invalid_format); /* noreply? */
    mc->noreply = 1;
    /* \r\n */
    GET_HOLD(buffer, mc->noreply_char, NOREPLY_LENGTH - 1, pos, bytes, db_error_invalid_format);
    NOT_EQUAL_RETURN(mc->noreply_char, MEMCACHED_CRLF, db_error_invalid_format); /* \r\n? */
    return db_error_ok;
}

/*
 * addspace <key>\r\n
 */
int memcache_analyzer_do_analyze_addspace(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int bytes = 0;
    /* key */
    GET_FORWARD(buffer, mc->key, MAX_PATH_SIZE - 1, pos, bytes, db_error_invalid_format);
    /* exptime */
    GET_FORWARD(buffer, mc->exptime_char, EXPTIME_LENGTH - 1, pos, bytes, db_error_invalid_format);
    EQUAL_RETURN(mc->exptime_char, MEMCACHED_CRLF, db_error_invalid_format); /* \r\n?*/
    mc->exptime = atol(mc->exptime_char);
    /* \r\n */
    GET_HOLD(buffer, mc->noreply_char, NOREPLY_LENGTH - 1, pos, bytes, db_error_invalid_format);
    NOT_EQUAL_RETURN(mc->noreply_char, MEMCACHED_CRLF, db_error_invalid_format); /* \r\n? */
    return db_error_ok;
}

int memcache_analyzer_analyze_command_prefix_a(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int error = db_error_ok;
    if (EQUAL(mc->command, ADD)) {
        mc->command_type = command_type_add;
        error = memcache_analyzer_do_analyze_store(mc, buffer, pos, 0);
    } else if (EQUAL(mc->command, ADDSPACE)) {
        mc->command_type = command_type_addspace;
        error = memcache_analyzer_do_analyze_addspace(mc, buffer, pos);
    } else if (EQUAL(mc->command, APPEND)) {
        mc->command_type = command_type_append;
        error = memcache_analyzer_do_analyze_store(mc, buffer, pos, 0);
    } else {
        error = db_error_unknown_command;
    }
    return error;
}

int memcache_analyzer_analyze_command_prefix_c(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int error = db_error_ok;
    if (EQUAL(mc->command, CAS)) {
        mc->command_type = command_type_cas;
        error = memcache_analyzer_do_analyze_store(mc, buffer, pos, 1);
    } else {
        error = db_error_unknown_command;
    }
    return error;
}

int memcache_analyzer_analyze_command_prefix_d(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int error = db_error_ok;
    if (EQUAL(mc->command, DELETE)) {
        mc->command_type = command_type_delete;
        error = memcache_analyzer_do_analyze_delete(mc, buffer, pos);
    } else if (EQUAL(mc->command, DELETESPACE)) {
        mc->command_type = command_type_deletespace;
        error = memcache_analyzer_do_analyze_delete(mc, buffer, pos);
    } else if (EQUAL(mc->command, DECR)) {
        mc->command_type = command_type_decr;
        error = memcache_analyzer_do_analyze_incr_decr(mc, buffer, pos);
    } else {
        error = db_error_unknown_command;
    }
    return error;
}

int memcache_analyzer_analyze_command_prefix_g(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int error = db_error_ok;
    if (EQUAL(mc->command, GET)) {
        mc->command_type = command_type_get;
        error = memcache_analyzer_do_analyze_get(mc, buffer, pos, 0);
    } else if (EQUAL(mc->command, GETS)) {
        mc->command_type = command_type_gets;
        error = memcache_analyzer_do_analyze_get(mc, buffer, pos, 1);
    } else {
        error = db_error_unknown_command;
    }
    return error;
}

int memcache_analyzer_analyze_command_prefix_l(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int error = db_error_ok;
    if (EQUAL(mc->command, LEAVEKEY)) {
        mc->command_type = command_type_leavekey;
        error = memcache_analyzer_do_analyze_leave(mc, buffer, pos);
    } else if (EQUAL(mc->command, LEAVE)) {
        mc->command_type = command_type_leave;
        error = memcache_analyzer_do_analyze_leave(mc, buffer, pos);
    } else {
        error = db_error_unknown_command;
    }
    return error;
}

int memcache_analyzer_analyze_command_prefix_i(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int error = db_error_ok;
    if (EQUAL(mc->command, INCR)) {
        mc->command_type = command_type_incr;
        error = memcache_analyzer_do_analyze_incr_decr(mc, buffer, pos);
    } else {
        error = db_error_unknown_command;
    }
    return error;
}

int memcache_analyzer_analyze_command_prefix_p(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int error = db_error_ok;
    if (EQUAL(mc->command, PREPEND)) {
        mc->command_type = command_type_prepend;
        error = memcache_analyzer_do_analyze_store(mc, buffer, pos, 0);
    } else {
        error = db_error_unknown_command;
    }
    return error;
}

int memcache_analyzer_analyze_command_prefix_q(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int error = db_error_ok;
    UNUSED(buffer);
    UNUSED(pos);
    if (EQUAL(mc->command, QUIT)) {
        mc->command_type = command_type_quit;
    } else {
        error = db_error_unknown_command;
    }
    return error;
}

int memcache_analyzer_analyze_command_prefix_r(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int error = db_error_ok;
    if (EQUAL(mc->command, REPLACE)) {
        mc->command_type = command_type_replace;
        error = memcache_analyzer_do_analyze_store(mc, buffer, pos, 0);
    } else {
        error = db_error_unknown_command;
    }
    return error;
}

int memcache_analyzer_analyze_command_prefix_s(memcache_analyzer_t* mc, const char* buffer, int pos) {
    int error = db_error_ok;
    if (EQUAL(mc->command, SET)) {
        mc->command_type = command_type_set;
        error = memcache_analyzer_do_analyze_store(mc, buffer, pos, 0);
    } else if (EQUAL(mc->command, SUBKEY)) {
        mc->command_type = command_type_subkey;
        error = memcache_analyzer_do_analyze_sub(mc, buffer, pos);
    } else if (EQUAL(mc->command, SUB)) {
        mc->command_type = command_type_sub;
        error = memcache_analyzer_do_analyze_sub(mc, buffer, pos);
    } else {
        error = db_error_unknown_command;
    }
    return error;
}

int memcache_analyzer_analyze_command_line(memcache_analyzer_t* mc, const char* buffer) {
    int pos   = 0;
    int error = db_error_ok;
    assert(mc);
    assert(buffer);
    pos = memcache_analyzer_command_line_get(buffer, mc->command, COMMAND_LENGTH - 1);
    if (!pos) {
        return db_error_invalid_format;
    }
    switch (mc->command[0]) {
        case 'a': { /* add, addspace, append */
            error = memcache_analyzer_analyze_command_prefix_a(mc, buffer, pos);
            break;
        }
        case 'c': { /* cas */
            error = memcache_analyzer_analyze_command_prefix_c(mc, buffer, pos);
            break;
        }
        case 'd': { /* delete, deletespace, decr */
            error = memcache_analyzer_analyze_command_prefix_d(mc, buffer, pos);
            break;
        }
        case 'g': { /* get, gets */
            error = memcache_analyzer_analyze_command_prefix_g(mc, buffer, pos);
            break;
        }
        case 'i': { /* incr */
            error = memcache_analyzer_analyze_command_prefix_i(mc, buffer, pos);
            break;
        }
        case 'l': { /* leavekey, leave */
            error = memcache_analyzer_analyze_command_prefix_l(mc, buffer, pos);
            break;
        }
        case 'p': { /* prepend */
            error = memcache_analyzer_analyze_command_prefix_p(mc, buffer, pos);
            break;
        }
        case 'q': { /* quit */
            error = memcache_analyzer_analyze_command_prefix_q(mc, buffer, pos);
            break;
        }
        case 'r': { /* replace */
            error = memcache_analyzer_analyze_command_prefix_r(mc, buffer, pos);
            break;
        }
        case 's': { /* set, subkey, sub */
            error = memcache_analyzer_analyze_command_prefix_s(mc, buffer, pos);
            break;
        }
        default: {
            error = db_error_unknown_command;
            break;
        }
    }
    return error;
}

/*
 * TODO:
 * command_type_append
 * command_type_prepend
 */

int memcache_analyzer_do_command(memcache_analyzer_t* mc, kchannel_ref_t* channel) {
    int error = db_error_ok;
    int i     = 0;
    switch (mc->command_type) {
        case command_type_quit: /* quit */
            knet_channel_ref_close(channel);
            break;
        case command_type_set: /* set */
            error = kdb_space_set_key(root_space, mc->key, mc->data, mc->bytes, mc->flags, mc->exptime);
            break;
        case command_type_add: /* add */
            error = kdb_space_add_key(root_space, mc->key, mc->data, mc->bytes, mc->flags, mc->exptime);
            break;
        case command_type_addspace: /* addspace */
            error = kdb_space_add_space(root_space, mc->key, mc->exptime);
            break;
        case command_type_replace: /* replace */
            error = kdb_space_update_key(root_space, mc->key, mc->data, mc->bytes, mc->flags, mc->exptime, 0);
            break;
        case command_type_cas: /* cas */
            error = kdb_space_cas_key(root_space, mc->key, mc->data, mc->bytes, mc->flags, mc->exptime, mc->cas_unique);
            break;
        case command_type_get: /* get */
            error = kdb_space_get_key(root_space, mc->key, &mc->return_value);
            break;
        case command_type_gets: /* gets */
            for (i = 0; i < mc->key_count; i++) {
                error = kdb_space_get_key(root_space, mc->keys[i], &mc->return_value_array[i]);
                if (db_error_ok != error) {
                    mc->return_value_array[i] = 0;
                }
            }
            error = db_error_ok;
            break;
        case command_type_delete: /* delete */
            error = kdb_space_del_key(root_space, mc->key);
            break;
        case command_type_deletespace: /* deletespace */
            error = kdb_space_del_space(root_space, mc->key);
            break;
        case command_type_subkey: /* subkey */
            error = kdb_space_subscribe_key(root_space, mc->key, channel);
            break;
        case command_type_sub: /* sub */
            error = kdb_space_subscribe(root_space, mc->key, channel);
            break;
        case command_type_leavekey: /* leavekey */
            error = kdb_space_forget_key(root_space, mc->key, channel);
            break;
        case command_type_leave: /* leave */
            error = kdb_space_forget(root_space, mc->key, channel);
            break;
        case command_type_incr: /* incr */
            error = kdb_space_incr_key(root_space, mc->key, mc->change_value, &mc->return_value);
            break;
        case command_type_decr: /* decr */
            error = kdb_space_decr_key(root_space, mc->key, -1 * mc->change_value, &mc->return_value);
            break;
        default:
            error = db_error_command_not_impl;
    }
    return error;
}

void memcache_analyzer_return_success(memcache_analyzer_t* mc, kchannel_ref_t* channel) {
    int          i      = 0;
    kdb_value_t* v      = 0;
    kstream_t*   stream = knet_channel_ref_get_stream(channel);
    switch (mc->command_type) {
        case command_type_set:
        case command_type_add:
        case command_type_addspace:
        case command_type_replace:
        case command_type_cas:
            knet_stream_push_varg(stream, STORED);
            break;
        case command_type_get:
            if (mc->return_value) {
                v = kdb_space_value_get_value(mc->return_value);
                knet_stream_push_varg(stream, VALUE_FORMAT, mc->key, kdb_value_get_cas_id(v), kdb_value_get_size(v));
                knet_stream_push(stream, kdb_value_get_value(v), kdb_value_get_size(v));
                knet_stream_push_varg(stream, MEMCACHED_CRLF);
            }
            knet_stream_push_varg(stream, END);
            break;
        case command_type_gets:
            for (i = 0; i < mc->key_count; i++) {
                if (mc->return_value_array[i]) {
                    v = kdb_space_value_get_value(mc->return_value_array[i]);
                    knet_stream_push_varg(stream, VALUE_FORMAT, mc->keys[i], kdb_value_get_cas_id(v), kdb_value_get_size(v));
                    knet_stream_push(stream, kdb_value_get_value(v), kdb_value_get_size(v));
                    knet_stream_push_varg(stream, MEMCACHED_CRLF);
                }
            }
            knet_stream_push_varg(stream, END);
            break;
        case command_type_incr:
        case command_type_decr:
            v = kdb_space_value_get_value(mc->return_value);
            knet_stream_push(stream, kdb_value_get_value(v), kdb_value_get_size(v));
            knet_stream_push_varg(stream, MEMCACHED_CRLF);
            break;
        case command_type_delete:
        case command_type_deletespace:
            knet_stream_push_varg(stream, DELETED);
            break;
        case command_type_subkey:
        case command_type_sub:
        case command_type_leavekey:
        case command_type_leave:
            knet_stream_push_varg(stream, STORED);
            break;
        default:
            break;
    }
}

void memcache_analyzer_return_error(memcache_analyzer_t* mc, kchannel_ref_t* channel, int error) {
    kstream_t* stream = knet_channel_ref_get_stream(channel);
    switch (mc->command_type) {
        case command_type_set:
        case command_type_add:
        case command_type_addspace:
        case command_type_replace:
            knet_stream_push_varg(stream, NOT_STORED);
            break;
        case command_type_cas:
            if (db_error_cas_fail == error) {
                knet_stream_push_varg(stream, EXIST);
            } else {
                knet_stream_push_varg(stream, NOT_FOUND);
            }
            break;
        case command_type_get:
        case command_type_gets:
            knet_stream_push_varg(stream, END);
            break;
        case command_type_delete:
        case command_type_deletespace:
        case command_type_subkey:
        case command_type_sub:
        case command_type_leavekey:
        case command_type_leave:
            knet_stream_push_varg(stream, NOT_FOUND);
            break;
        case command_type_incr:
        case command_type_decr:
            knet_stream_push_varg(stream, NOT_FOUND);
            break;
        default:
            break;
    }
}

void memcache_analyzer_return(memcache_analyzer_t* mc, kchannel_ref_t* channel, int error) {
    kstream_t* stream = knet_channel_ref_get_stream(channel);
    if (mc->noreply) {
        return;
    }
    switch (error) {
        case db_error_ok:
            memcache_analyzer_return_success(mc, channel);
            break;
        case db_error_invalid_format:
            knet_stream_push_varg(stream, CLIENT_ERROR_FORMAT, INVALID_COMMAND_FORMAT, mc->command_line);
            break;
        case db_error_unknown_command:
            knet_stream_push_varg(stream, ERROR);
            break;
        case db_error_command_not_impl:
            knet_stream_push_varg(stream, SERVER_ERROR_FORMAT1, COMMOND_NOT_IMPLEMENTED, mc->command);
            break;
        case db_error_incr_decr_fail:
            knet_stream_push_varg(stream, SERVER_ERROR_FORMAT2, mc->command_line);
            break;
        default:
            memcache_analyzer_return_error(mc, channel, error);
            break;
    }
}

int publish_update(kchannel_ref_t* channel, kdb_space_value_t* sv) {
    kstream_t*   stream = knet_channel_ref_get_stream(channel);
    kdb_value_t* dv     = kdb_space_value_get_value(sv);
    kdb_space_t* owner  = kdb_space_value_get_owner(sv);
    knet_stream_push_varg(stream, "PUB UPDATED %s.%s %d\r\n", kdb_space_get_path(owner),
        kdb_space_value_get_name(sv), kdb_value_get_size(dv));
    knet_stream_push(stream, kdb_value_get_value(dv), kdb_value_get_size(dv));
    knet_stream_push_varg(stream, CRLF_END_CRLF);
    return db_error_ok;
}

int publish_delete(kchannel_ref_t* channel, kdb_space_value_t* sv) {
    kstream_t*   stream = knet_channel_ref_get_stream(channel);
    kdb_value_t* dv     = 0;
    kdb_space_t* owner  = kdb_space_value_get_owner(sv);
    if (kdb_space_value_check_type(sv, space_value_type_space)) {
        knet_stream_push_varg(stream, "PUB DELETED SPACE %s\r\n",
            kdb_space_get_path(kdb_space_value_get_space(sv)));
    } else if (kdb_space_value_check_type(sv, space_value_type_value)) {
        dv = kdb_space_value_get_value(sv);
        knet_stream_push_varg(stream, "PUB DELETED OBJECT %s.%s %d\r\n", kdb_space_get_path(owner),
            kdb_space_value_get_name(sv), kdb_value_get_size(dv));
        knet_stream_push(stream, kdb_value_get_value(dv), kdb_value_get_size(dv));
        knet_stream_push_varg(stream, CRLF_END_CRLF);
    }    
    return db_error_ok;
}

int publish_add(kchannel_ref_t* channel, kdb_space_value_t* sv) {
    kstream_t*   stream = knet_channel_ref_get_stream(channel);
    kdb_value_t* dv     = 0;
    kdb_space_t* owner  = kdb_space_value_get_owner(sv);
    if (kdb_space_value_check_type(sv, space_value_type_space)) {
        knet_stream_push_varg(stream, "PUB ADDED SPACE %s\r\n",
            kdb_space_get_path(kdb_space_value_get_space(sv)));
    } else if (kdb_space_value_check_type(sv, space_value_type_value)) {
        dv = kdb_space_value_get_value(sv);
        knet_stream_push_varg(stream, "PUB ADDED OBJECT %s.%s %d\r\n", kdb_space_get_path(owner),
            kdb_space_value_get_name(sv), kdb_value_get_size(dv));
        knet_stream_push(stream, kdb_value_get_value(dv), kdb_value_get_size(dv));
        knet_stream_push_varg(stream, CRLF_END_CRLF);
    }    
    return db_error_ok;
}

int memcache_analyzer_analyze(memcache_analyzer_t* mc, kchannel_ref_t* channel) {
    int        size   = COMMAND_LINE_LENGTH;
    int        error  = db_error_ok;
    int        need   = 0;
    kstream_t* stream = 0;
    assert(mc);
    assert(channel);
    stream = knet_channel_ref_get_stream(channel);
    if (!mc->bytes) {
        if (error_ok != knet_stream_pop_until(stream, MEMCACHED_CRLF, mc->command_line, &size)) {
            return db_error_channel_need_more;
        }
        mc->command_line[size] = 0;
        /* ��ȡ���������� */
        error = memcache_analyzer_analyze_command_line(mc, mc->command_line);
    } else {
        need += mc->bytes - mc->recv_bytes; /* bytes������\r\n���� */
        if (knet_stream_available(stream) >= need) {
            error = knet_stream_pop(stream, mc->data + mc->recv_bytes, need);
            if (error_ok == error) {
                mc->recv_bytes += need;
                mc->bytes      -= 2;
            } else {
                /* ͨ�����ᷢ�� */
                knet_channel_ref_close(channel);
            }
        } else {
            /* ���ݲ��� */
            return db_error_channel_need_more;
        }
    }
    if (db_error_ok == error) {
        if (mc->bytes && (mc->bytes > mc->recv_bytes)) { /* ��δ��ȡ�������� */
            return db_error_channel_need_more;
        }
        /* �������� */
        error = memcache_analyzer_do_command(mc, channel);
    }
    /* ������ */
    memcache_analyzer_return(mc, channel, error);
    /* ���� */
    memcache_analyzer_reset(mc);
    return error;
}

void memcache_analyzer_reset(memcache_analyzer_t* mc) {
    assert(mc);
    mc->bytes        = 0;
    mc->flags        = 0;
    mc->noreply      = 0;
    mc->recv_bytes   = 0;
    mc->exptime      = 0;
    mc->command_type = 0;
    mc->cas_unique   = 0;
    mc->key_count    = 0;
    mc->change_value = 0;
    mc->return_value = 0;
}

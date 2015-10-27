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

#include "knet.h"
#include "db_common.h"
#include "memcache_client_analyzer.h"

struct _memcache_client_analyzer_t {
    char               command_line[COMMAND_LINE_LENGTH];      /* 命令行 */
    char               command[COMMAND_LENGTH];                /* 命令 */
    char               keys[MAX_KEYS][MAX_PATH_SIZE];          /* 多键 */
    int                key_count;                              /* gets键的数量 */
    char               data[ACTION_BUFFER_LENGTH];             /* 值 */
    int                recv_bytes;                             /* 当前值已经接收到的字节 */
    char               bytes_char[BYTES_LENGTH];               /* 值长度 */
    int                bytes;                                  /* 值长度 */
    uint64_t           cas_uniques[MAX_KEYS];                  /* memcached CAS unique ID */
};

memcache_client_analyzer_t* memcache_client_analyzer_create() {
    memcache_client_analyzer_t* analyzer = create(memcache_client_analyzer_t);
    assert(analyzer);
    memset(analyzer, 0, sizeof(memcache_client_analyzer_t));
    return analyzer;
}

void memcache_client_analyzer_destroy(memcache_client_analyzer_t* analyzer) {
    assert(analyzer);
    destroy(analyzer);
}

/*
 * PUB UPDATED <key> <bytes>\r\n
 * data\r\n
 * END\r\n
 * 
 * PUB DELETED SPACE <key>\r\n
 *
 * PUB DELETED OBJECT <key> <bytes>\r\n
 * data\r\n
 * END\r\n
 *
 * PUB ADDED SPACE <key>\r\n
 *
 * PUB ADDED OBJECT <key> <bytes>\r\n
 * data\r\n
 * END\r\n
 */
int memcache_client_analyzer_analyze_prefix_P(memcache_client_analyzer_t* analyzer, kstream_t* stream, const char* key, char* buffer) {
    char command[MAX_PATH_SIZE] = {0};
    int pos = strlen(key);
    int bytes = 0;
    GET_FORWARD(buffer + pos, command, MAX_PATH_SIZE - 1, pos, bytes, db_client_invalid_format);
    NOT_EQUAL_RETURN(command, "PUB", db_client_unknown_command);
    GET_FORWARD(buffer + pos, command, MAX_PATH_SIZE - 1, pos, bytes, db_client_invalid_format);
    if (EQUAL("UPDATED", command)) {
    } else if (EQUAL("DELETED", command)) {
    } else if (EQUAL("ADDED", command)) {
    }
    return db_client_ok;
}

/*
 * STORED
 * SERVER_ERROR
 */
int memcache_client_analyzer_analyze_prefix_S(memcache_client_analyzer_t* analyzer, kstream_t* stream, const char* key, char* buffer) {
    if (EQUAL("STORED", key)) {
        return db_client_ok;
    } else if (EQUAL("SERVER_ERROR", key)) {
        return db_client_command_error;
    }
    return db_client_unknown_command;
}

/*
 * CLIENT_ERROR
 */
int memcache_client_analyzer_analyze_prefix_C(memcache_client_analyzer_t* analyzer, kstream_t* stream, const char* key, char* buffer) {
    if (EQUAL("CLIENT_ERROR", key)) {
        return db_client_command_error;
    }
    return db_client_unknown_command;
}

/*
 * ERROR, EXIST
 */
int memcache_client_analyzer_analyze_prefix_E(memcache_client_analyzer_t* analyzer, kstream_t* stream, const char* key, char* buffer) {
    if (EQUAL("ERROR", key)) {
        return db_client_command_error;
    } else if (EQUAL("EXIST", key)) {
        return db_client_command_error;
    }
    return db_client_unknown_command;
}

/*
 * NOT_FOUND
 */
int memcache_client_analyzer_analyze_prefix_N(memcache_client_analyzer_t* analyzer, kstream_t* stream, const char* key, char* buffer) {
    if (EQUAL("NOT_FOUND", key)) {
        return db_client_command_error;
    }
    return db_client_unknown_command;
}

/*
 * VALUE <key> <casid> <bytes>\r\n<data>\r\n
 */
int memcache_client_analyzer_analyze_prefix_V(memcache_client_analyzer_t* analyzer, kstream_t* stream, const char* key, char* buffer) {
    char command[MAX_PATH_SIZE] = {0};
    int pos = strlen(key);
    int bytes = 0;
    GET_FORWARD(buffer + pos, command, MAX_PATH_SIZE - 1, pos, bytes, db_client_invalid_format);
    if (!EQUAL("VALUE", key)) {
        return db_client_unknown_command;
    }
    // TODO key
    GET_FORWARD(buffer + pos, command, MAX_PATH_SIZE - 1, pos, bytes, db_client_invalid_format);
    // TODO casid
    GET_FORWARD(buffer + pos, command, MAX_PATH_SIZE - 1, pos, bytes, db_client_invalid_format);
    // TODO bytes
    GET_FORWARD(buffer + pos, command, MAX_PATH_SIZE - 1, pos, bytes, db_client_invalid_format);
    // TODO get data
    return db_client_ok;
}

/*
 * DELETED
 */
int memcache_client_analyzer_analyze_prefix_D(memcache_client_analyzer_t* analyzer, kstream_t* stream, const char* key, char* buffer) {
    if (EQUAL("DELETED", key)) {
        return db_client_ok;
    }
    return db_client_unknown_command;
}

int memcache_client_analyzer_do_task(memcache_client_analyzer_t* analyzer, kchannel_ref_t* channel, kdb_client_task_t* task) {
    int error = error_ok;
    kstream_t* stream = knet_channel_ref_get_stream(channel);
    char buffer[1024] = {0};
    int size = sizeof(buffer);
    error = knet_stream_pop_until(stream, CRLF, buffer, &size);
    if (error != error_ok) {
        return db_client_need_more;
    }
    char key[32] = {0};
    memcache_analyzer_command_line_get(buffer, key, sizeof(key));
    switch (key[0]) {
        case 'C': /* CLIENT_ERROR */
            return memcache_client_analyzer_analyze_prefix_C(analyzer, stream, key, buffer);
        case 'D': /* DELETED */
            return memcache_client_analyzer_analyze_prefix_D(analyzer, stream, key, buffer);
        case 'E': /* ERROR, EXIST */
            return memcache_client_analyzer_analyze_prefix_E(analyzer, stream, key, buffer);
        case 'P': /* PUB */
            return memcache_client_analyzer_analyze_prefix_P(analyzer, stream, key, buffer);
        case 'N': /* NOT_FOUND */
            return memcache_client_analyzer_analyze_prefix_N(analyzer, stream, key, buffer);
        case 'S': /* STORED, SERVER_ERROR */
            return memcache_client_analyzer_analyze_prefix_S(analyzer, stream, key, buffer);
        case 'V': /* VALUE */
            return memcache_client_analyzer_analyze_prefix_V(analyzer, stream, key, buffer);
    }
    return db_client_ok;
}

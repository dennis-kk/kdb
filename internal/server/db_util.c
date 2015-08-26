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

#include "db_util.h"
#include "db_server.h"

int isnumber(void* s, int size) {
    char* p = (char*)s;
    if (*p == '-') {
        p++;
        size--;
    }
    while ((*p >= '0') && (*p <= '9') && size) {
        p++;
        size--;
    }
    return (size ? 0 : 1);
}

long long atoll_s(void* s, int size) {  
    int       minus = 0;
    long long value = 0;
    char*     p     = (char*)s;
    if (*p == '-') {
        minus++;
        p++;
        size--;
    }
    while ((*p >= '0') && (*p <= '9') && size) {
        value *= 10;
        value += *p - '0';
        p++;
        size--;
    }
    return minus ? 0 - value : value;
}

char* kdb_lltoa(long long ll, char* buffer, int* size) {
    int len = 0;
    assert(buffer);
    assert(size);
#if defined(WIN32)
    len = _snprintf(buffer, *size, "%lld", ll);
    if ((len >= *size) || (len < 0)) {
        return 0;
    }
#else
    len = snprintf(buffer, *size, "%lld", ll);
    if (len <= 0) {
        return 0;
    }
#endif /* WIN32 */
    buffer[len] = 0;
    *size = len;
    return buffer;
}

void* kdb_malloc(int size) {
    if (db_server) {
        if (kdb_server_get_malloc(db_server)) {
            return kdb_server_get_malloc(db_server)(size);
        }
    }
    return create_raw(size);
}

void* kdb_realloc(void* p, int size) {
    if (db_server) {
        if (kdb_server_get_realloc(db_server)) {
            return kdb_server_get_realloc(db_server)(p, size);
        }
    }
    return rcreate_raw(p, size);
}

void kdb_free(void* p) {
    if (db_server) {
        if (kdb_server_get_free(db_server)) {
            kdb_server_get_free(db_server)(p);
        }
    }
    destroy(p);
}

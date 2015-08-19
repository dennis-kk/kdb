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

#include "db_space.h"
#include "db_server.h"
#include "memcache_analyzer.h"

/**
 * �ռ�ֵ����
 */
enum _db_space_value_type_e {
    space_value_type_value = 1, /* ���� */
    space_value_type_space = 2, /* �ռ� */
};

/**
 * ֵ�仯��������
 */
enum _db_sub_type_e {
    kdb_sub_type_update = 1, /* ���� */
    kdb_sub_type_delete = 2, /* ���� */
};

/**
 * ֵ
 */
struct _db_value_t {
    void*     v;            /* ���� */
    int       size;         /* ��ǰ���� */
    int       max_size;     /* ��󳤶� */
    khash_t*  sub_channels; /* �����˴�����/�ռ�Ĺܵ� */
    uint32_t  flags;        /* memcached flags */
    uint64_t  cas_id;       /* memcached cas unique */
};

/**
 * �ռ�ֵ
 */
struct _db_space_value_t {
    uint64_t    id;        /* UUID */
    uint32_t    exptime;   /* memcached exptime*/
    kdb_space_t* owner;     /* �����ռ� */
    char*       name;      /* �ռ�/ֵ���� */
    union {
        kdb_value_t* value; /* ���� */
        kdb_space_t* space; /* �ռ� */
    };
    kdb_space_value_type_e type; /* ���� */
    void* ptr; /* �û�ָ�룬���ڲ�� */
};

/**
 * �ռ�
 */
struct _db_space_t {
    int          buckets; /* ��ϣ��Ͱ������ */
    khash_t*     h;       /* ��ϣ�� */
    kdb_space_t*  parent;  /* ���ռ� */
    kdb_server_t* srv;     /* ������ */
    char*        path;    /* �ռ�ȫ·�� */
};

kdb_value_t* kdb_value_create(const void* value, int size) {
    kdb_value_t* v = create(kdb_value_t);
    memset(v, 0, sizeof(kdb_value_t));
    v->v = create_raw(size);
    memcpy(v->v, value, size);
    v->size     = size;
    v->max_size = size;
    return v;
}

void kdb_value_destroy(kdb_value_t* value) {
    assert(value);    
    /* ���� */
    if (value->v) {
        destroy(value->v);
    }
    destroy(value);
}

void* kdb_value_get_value(kdb_value_t* value) {
    assert(value);
    return value->v;
}

int kdb_value_get_size(kdb_value_t* value) {
    assert(value);
    return value->size;
}

uint32_t kdb_value_get_flags(kdb_value_t* value) {
    assert(value);
    return value->flags;
}

uint64_t kdb_value_get_cas_id(kdb_value_t* value) {
    assert(value);
    return value->cas_id;
}

int kdb_value_subscribe(kdb_value_t* value, kchannel_ref_t* channel) {
    uint32_t id = 0;
    assert(value);
    assert(channel);
    if (!value->sub_channels) {
        value->sub_channels = hash_create(8, sub_dtor);
        assert(value->sub_channels);
    }
    /* ȡ�ùܵ�����ID */
    id = uuid_get_high32(knet_channel_ref_get_uuid(channel));
    knet_channel_ref_incref(channel); /* �������ü��� */
    if (error_ok != hash_replace(value->sub_channels, id, channel)) {
        return db_error_sub_fail;
    }
    return db_error_ok;
}

int kdb_space_value_forget(kdb_space_value_t* value, kchannel_ref_t* channel) {
    uint32_t id = 0;
    assert(value);
    assert(channel);
    if (!value->value->sub_channels) {
        return db_error_path_not_found;
    }
    /* ȡ�ùܵ�����ID */
    id = uuid_get_high32(knet_channel_ref_get_uuid(channel));
    if (error_ok != hash_delete(value->value->sub_channels, id)) {
        return db_error_hash_delete;
    }
    return db_error_ok;
}

void kdb_space_value_publish(kdb_space_value_t* value, kdb_sub_type_e type) {
    khash_value_t*  hv      = 0;
    kchannel_ref_t* channel = 0;
    int             error   = db_error_ok;
    if (value->value->sub_channels) {
        /* �����ж��������� */
        hash_for_each_safe(value->value->sub_channels, hv) {
            channel = (kchannel_ref_t*)hash_value_get_value(hv);
            if (type == kdb_sub_type_update) { /* ���� */
                error = publish_update(channel, value->owner->path, value);
            } else if (type == kdb_sub_type_delete) { /* ���� */
                error = publish_delete(channel, value->owner->path);
            }
            if (db_error_ok != error) { /* �ܵ�ʧЧ, ȡ������ */
                kdb_space_value_forget(value, channel);
            }
        }
    }
}

kdb_value_t* kdb_space_value_get_value(kdb_space_value_t* value) {
    if (kdb_space_value_check_type(value, space_value_type_value)) {
        return value->value;
    }
    return 0;
}

kdb_space_value_t* kdb_space_value_create_value(kdb_space_t* owner, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime) {
    kdb_space_value_t* v = create(kdb_space_value_t);
    assert(v);
    memset(v, 0, sizeof(kdb_space_value_t));
    v->type  = space_value_type_value;
    v->owner = owner;
    v->id    = uuid_create(); /* UUID */
    v->name  = create_raw(strlen(name) + 1);
    assert(v->name);
    strcpy(v->name, name); /* ֵ���� */
    v->value = kdb_value_create(value, size);
    assert(v->value);
    v->value->flags = flags;
    v->exptime      = exptime;
    return v;
}

kdb_space_value_t* kdb_space_value_create_space(kdb_space_t* owner, const char* full_path, const char* name, uint32_t exptime) {
    kdb_space_value_t* v   = 0;
    kdb_server_t*      srv = 0;
    assert(owner);
    v = create(kdb_space_value_t);
    assert(v);
    srv = kdb_space_get_server(owner);
    memset(v, 0, sizeof(kdb_space_value_t));
    v->type  = space_value_type_space;
    v->owner = owner;
    v->id    = uuid_create(); /* UUID */
    v->name  = create_raw(strlen(name) + 1); /* �ռ����� */
    assert(v->name);
    strcpy(v->name, name);
    v->space = kdb_space_create(owner, srv, kdb_server_get_space_buckets(db_server));
    assert(v->space);
    v->space->path = create_raw(strlen(full_path) + 1); /* ȫ·�� */
    assert(v->space->path);
    strcpy(v->space->path, full_path);
    v->exptime = exptime;
    return v;
}

void kdb_space_value_destroy(kdb_space_value_t* v) {
    assert(v);
    if (v->type == space_value_type_value) {
        /* ����ֵ�����¼� */
        kdb_space_value_publish(v, kdb_sub_type_delete);
        kdb_server_call_cb_on_delete(db_server, v);
        kdb_value_destroy(v->value);
    } else if (v->type == space_value_type_space) { /* �ռ�����ٲ����� */
        kdb_space_destroy(v->space);
    }
    destroy(v->name);
    destroy(v);
}

int kdb_space_value_check_type(kdb_space_value_t* v, kdb_space_value_type_e type) {
    assert(v);
    return (v->type == type);
}

void kdb_space_value_set_ptr(kdb_space_value_t* v, void* ptr) {
    assert(v);
    v->ptr = ptr;
}

void* kdb_space_value_get_ptr(kdb_space_value_t* v) {
    assert(v);
    return v->ptr;
}

int kdb_space_value_subscribe_space(kdb_space_value_t* v, kchannel_ref_t* channel) {
    khash_value_t*    hv    = 0;
    kdb_space_value_t* sv    = 0;
    int               error = db_error_ok;
    assert(v);
    assert(channel);
    hash_for_each_safe(v->space->h, hv) {
        sv = (kdb_space_value_t*)hash_value_get_value(hv);
        if (kdb_space_value_check_type(sv, space_value_type_value)) {
            error = kdb_value_subscribe(sv->value, channel);            
        } else if (kdb_space_value_check_type(sv, space_value_type_space)) {
            error = kdb_space_value_subscribe_space(sv, channel);
        }
        if (db_error_ok != error) {
            return error;
        }
    }
    return error;
}

int kdb_space_value_forget_space(kdb_space_value_t* v, kchannel_ref_t* channel) {
    khash_value_t*    hv    = 0;
    kdb_space_value_t* sv    = 0;
    int               error = db_error_ok;
    assert(v);
    assert(channel);
    hash_for_each_safe(v->space->h, hv) {
        sv = (kdb_space_value_t*)hash_value_get_value(hv);
        if (kdb_space_value_check_type(sv, space_value_type_value)) {
            error = kdb_space_value_forget_space(sv, channel);            
        } else if (kdb_space_value_check_type(sv, space_value_type_space)) {
            error = kdb_space_value_forget_space(sv, channel);
        }
        if (db_error_ok != error) {
            return error;
        }
    }
    return error;
}

void value_dtor(void* v) {
    kdb_space_value_t* value = (kdb_space_value_t*)v;
    kdb_space_value_destroy(value);
}

void sub_dtor(void* v) {
    kchannel_ref_t* channel = (kchannel_ref_t*)v;
    knet_channel_ref_decref(channel);
}

int kdb_iterate_path(kdb_space_t* space, const char* path, char* name, int* name_len) {
    int  key_len = 0;
    char c       = 0;
    int  i       = 0;
    assert(space);
    assert(path);
    assert(name);
    assert(name_len);
    *name_len = 0;
    for_each_char(c, path) {
        key_len += 1;
        if (c != DOT) {
            if (key_len > MAX_NAME_SIZE) {
                *name_len = 0;
                return db_error_key_name_overflow;
            }
            name[i++] = c;
            *name_len += 1;
        } else {
            if (!i) { /* ��ʽ���� */
                return db_error_invalid_format;
            }
            name[i]   = 0;
            *name_len = i + 1;
            break;
        }
    }
    return db_error_ok;
}

int kdb_space_set_key_path(kdb_space_t* space, const char* path, const char* full_path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime) {
    kdb_space_value_t* v     = 0;
    int               error = db_error_ok;
    assert(space);
    assert(path);
    assert(name);
    assert(value);
    assert(size);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (v) {
        if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
            if (*path) { /* �����Ѿ���һ������, �����ظ�������; */
                return db_error_invalid_path;
            }
            /* �Ѿ����� */
            return db_error_exist;
        } else if (kdb_space_value_check_type(v, space_value_type_space)) {
            if (*path) { /* ��������ӿռ� */
                return kdb_space_set_key(v->space, path, full_path, value, size, flags, exptime);
            }
            /* �ظ�������; */
            return db_error_invalid_path;
        }
        /* δ��������� */
        return db_error_invalid_type;
    } else {
        if (!*path) { /* ����, ���(path,value) */
            v = kdb_space_value_create_value(space, name, value, size, flags, exptime);
        } else { /* ����һ��space */
            v = kdb_space_value_create_space(space, full_path, name, exptime);
        }
        assert(v);
        /* �������� */
        error = hash_add_string_key(space->h, name, v);
        if (error_ok != error) {
            return db_error_hash_insert;
        }
        if (kdb_space_value_check_type(v, space_value_type_space)) {
            return kdb_space_set_key(v->space, path, full_path, value, size, flags, exptime);
        } else {
            kdb_server_call_cb_on_add(db_server, v);
        }
    }
    return db_error_ok;
}

int kdb_space_add_key_path(kdb_space_t* space, const char* path, const char* full_path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime) {
    kdb_space_value_t* v     = 0;
    int               error = db_error_ok;
    assert(space);
    assert(path);
    assert(name);
    assert(value);
    assert(size);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (!v) {
        if (!*path) { /* ����, ���(path,value) */
            v = kdb_space_value_create_value(space, name, value, size, flags, exptime);
        } else { /* δ������û���ҵ��ڵ� */
            return db_error_invalid_path;
        }
        assert(v);
        /* �������� */
        error = hash_add_string_key(space->h, name, v);
        if (error_ok != error) {
            return db_error_hash_insert;
        }
        kdb_server_call_cb_on_add(db_server, v);
        return db_error_ok;
    }
    if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
        if (*path) { /* �����Ѿ���һ������, �����ظ�������; */
            return db_error_invalid_path;
        }
        /* �Ѿ����� */
        return db_error_exist;
    } else if (kdb_space_value_check_type(v, space_value_type_space)) {
        if (*path) { /* ��������ӿռ� */
            return kdb_space_add_key(v->space, path, full_path, value, size, flags, exptime);
        }
        /* �ظ�������; */
        return db_error_invalid_path;
    }
    /* δ��������� */
    return db_error_invalid_type;   
}

int kdb_space_get_key_path(kdb_space_t* space, const char* path, const char* name, kdb_space_value_t** value) {
    kdb_space_value_t* v = 0;
    assert(space);
    assert(path);
    assert(name);
    assert(value);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (!v) {
        return db_error_path_not_found;
    }
    if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
        /* ��Ҫһ���ռ��������������һ������ */
        if (*path) {
            return db_error_path_need_space_but_value;
        }
        /* �ҵ� */
        *value = v;
        return db_error_ok;
    } else if (kdb_space_value_check_type(v, space_value_type_space)) {
        if (!*path) { /* ��δ�������ͽ�����һ���ռ�ڵ� */
            return db_error_path_not_found;
        }
        return kdb_space_get_key(v->space, path, value);
    }
    /* δ��������� */
    return db_error_invalid_type;    
}

int kdb_space_del_key_path(kdb_space_t* space, const char* path, const char* name) {
    kdb_space_value_t* v = 0;
    assert(space);
    assert(path);
    assert(name);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (!v) {
        return db_error_path_not_found;
    }
    if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
        /* ��Ҫһ���ռ��������������һ������ */
        if (*path) {
            return db_error_path_need_space_but_value;
        }
        /* �ҵ� */
        if (error_ok != hash_delete_string_key(space->h, name)) {
            return db_error_hash_delete;
        }
        return db_error_ok;
    } else if (kdb_space_value_check_type(v, space_value_type_space)) {
        if (!*path) { /* ��δ�������ͽ�����һ���ռ�ڵ� */
            return db_error_path_not_found;
        }
        return kdb_space_del_key(v->space, path);
    }
    /* δ��������� */
    return db_error_invalid_type;    
}

int kdb_space_update_key_path(kdb_space_t* space, const char* path, const char* name, const void* value, int size, uint32_t flags, uint32_t exptime, uint64_t cas_id) {
    kdb_space_value_t* v     = 0;
    int               error = db_error_ok;
    assert(space);
    assert(path);
    assert(name);
    assert(value);
    assert(size);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (!v) {
        return db_error_invalid_path;
    }
    if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
        if (*path) { /* δ����·��ĩβ */
            return db_error_invalid_path;
        }
        if (v->value->cas_id && !cas_id) { /* ����Ƿ���CAS���� */
            if (v->value->cas_id != cas_id) {
                return db_error_cas_fail;
            }            
        }
        /* ����CASID */
        if (cas_id) {
            v->value->cas_id = cas_id;
        }
        v->value->size  = size;
        v->value->flags = flags;        
        /* TODO ���¼������ʱ�� */
        v->exptime = exptime;
        /* ���� */
        if (v->value->max_size >= size) {
            /* �ظ�ʹ��ԭ�пռ� */
            memcpy(v->value->v, value, size);
        } else {
            /* ���� */
            v->value->max_size = size;
            v->value->v = rcreate_raw(v->value->v, size);
            assert(v->value->v);
            memcpy(v->value->v, value, size);
        }
        /* ���������¼� */
        kdb_space_value_publish(v, kdb_sub_type_update);
        kdb_server_call_cb_on_update(db_server, v);
        return error;
    } else if (kdb_space_value_check_type(v, space_value_type_space)) {
        if (*path) { /* ��������ӿռ� */
            return kdb_space_update_key(v->space, path, value, size, flags, exptime);
        }
        /* �ظ�������; */
        return db_error_invalid_path;
    }
    /* δ��������� */
    return db_error_invalid_type;   
}

int kdb_space_subscribe_key_path(kdb_space_t* space, const char* path, const char* name, kchannel_ref_t* channel) {
    kdb_space_value_t* v = 0;
    assert(space);
    assert(path);
    assert(channel);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (!v) {
        return db_error_path_not_found;
    }
    if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
        /* ��Ҫһ���ռ��������������һ������ */
        if (*path) {
            return db_error_path_need_space_but_value;
        }
        /* ���� */
        return kdb_value_subscribe(v->value, channel);
    } else if (kdb_space_value_check_type(v, space_value_type_space)) {
        if (!*path) { /* ��δ�������ͽ�����һ���ռ�ڵ� */
            return db_error_path_not_found;
        }
        return kdb_space_subscribe_key(v->space, path, channel);
    }
    /* δ��������� */
    return db_error_invalid_type;   
}

int kdb_space_add_space_path(kdb_space_t* space, const char* path, const char* full_path, const char* name, uint32_t exptime) {
    kdb_space_value_t* v     = 0;
    int               error = db_error_ok;
    assert(space);
    assert(path);
    assert(name);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (v) {
        if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
            /* ·���в��ܴ������� */
            return db_error_exist;
        } else if (kdb_space_value_check_type(v, space_value_type_space)) {
            if (*path) { /* ��������ӿռ� */
                return kdb_space_add_space(v->space, path, full_path, exptime);
            }
            /* �Ѿ����� */
            return db_error_invalid_path;
        }
        /* δ��������� */
        return db_error_invalid_type;
    } else {
        v = kdb_space_value_create_space(space, full_path, name, exptime);
        assert(v);
        /* �������� */
        error = hash_add_string_key(space->h, name, v);
        if (error_ok != error) {
            return db_error_hash_insert;
        }
        kdb_server_call_cb_on_add(db_server, v);
        if (*path) { /* ��δ���� */
            return kdb_space_add_space(v->space, path, full_path, exptime);
        }
    }
    return db_error_ok;
}

int kdb_space_get_space_path(kdb_space_t* space, const char* path, const char* name, kdb_space_t** child) {
    kdb_space_value_t* v = 0;
    assert(space);
    assert(path);
    assert(name);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (!v) {
        return db_error_path_not_found;
    }
    if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
        return db_error_invalid_type;
    } else if (kdb_space_value_check_type(v, space_value_type_space)) {
        if (*path) { /* ��δ�������������� */
            return kdb_space_get_space(v->space, path, child);
        }
        *child = v->space;
        return db_error_ok;
    }
    /* δ��������� */
    return db_error_invalid_type;
}

int kdb_space_del_space_path(kdb_space_t* space, const char* path, const char* name) {
    kdb_space_value_t* v = 0;
    assert(space);
    assert(path);
    assert(name);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (!v) {
        return db_error_path_not_found;
    }
    if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
        return db_error_invalid_type;
    } else if (kdb_space_value_check_type(v, space_value_type_space)) {
        if (*path) { /* ��δ�������������� */
            return kdb_space_del_space(v->space, path);
        }
        kdb_server_call_cb_on_delete(db_server, v);
        if (error_ok != hash_delete_string_key(space->h, name)) {
            return db_error_hash_delete;
        }
        return db_error_ok;
    }
    /* δ��������� */
    return db_error_invalid_type;    
}

int kdb_space_subscribe_space_path(kdb_space_t* space, const char* path, const char* name, kchannel_ref_t* channel) {
    kdb_space_value_t* v  = 0;
    assert(space);
    assert(path);
    assert(channel);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (!v) {
        return db_error_path_not_found;
    }
    if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
        /* ��Ҫһ���ռ��������������һ������ */
        return db_error_path_need_space_but_value;
    } else if (kdb_space_value_check_type(v, space_value_type_space)) { /* �ռ� */
        if (*path) { /* ��δ������������ */
            return kdb_space_subscribe(v->space, path, channel);
        }
        /* �ҵ������������ռ� */
        return kdb_space_value_subscribe_space(v, channel);
    }
    /* δ��������� */
    return db_error_invalid_type;
}

int kdb_space_forget_key_path(kdb_space_t* space, const char* path, const char* name, kchannel_ref_t* channel) {
    kdb_space_value_t* v = 0;
    assert(space);
    assert(path);
    assert(channel);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (!v) {
        return db_error_path_not_found;
    }
    if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
        /* ��Ҫһ���ռ��������������һ������ */
        if (*path) {
            return db_error_path_need_space_but_value;
        }
        /* ȡ������ */
        return kdb_space_value_forget(v, channel);
    } else if (kdb_space_value_check_type(v, space_value_type_space)) {
        if (!*path) { /* ��δ�������ͽ�����һ���ռ�ڵ� */
            return db_error_path_not_found;
        }
        return kdb_space_forget_key(v->space, path, channel);
    }
    /* δ��������� */
    return db_error_invalid_type;    
}

int kdb_space_forget_space_path(kdb_space_t* space, const char* path, const char* name, kchannel_ref_t* channel) {
    kdb_space_value_t* v  = 0;
    assert(space);
    assert(path);
    assert(channel);
    /* �鿴�Ƿ���� */
    v = (kdb_space_value_t*)hash_get_string_key(space->h, name);
    if (!v) {
        return db_error_path_not_found;
    }
    if (kdb_space_value_check_type(v, space_value_type_value)) { /* ��һ������ */
        /* ��Ҫһ���ռ��������������һ������ */
        if (*path) {
            return db_error_path_need_space_but_value;
        }
        /* ȡ������ */
        return kdb_space_value_forget(v, channel);
    } else if (kdb_space_value_check_type(v, space_value_type_space)) {
        if (!*path) { /* ������һ���ռ�ڵ� */
            /* �ռ����������� */
            return kdb_space_value_forget_space(v, channel);
        }
        return kdb_space_forget(v->space, path, channel);
    }
    /* δ��������� */
    return db_error_invalid_type;
}

kdb_space_t* kdb_space_create(kdb_space_t* parent, kdb_server_t* srv, int buckets) {
    kdb_space_t* space = create(kdb_space_t);
    memset(space, 0, sizeof(kdb_space_t));
    assert(space);
    space->parent  = parent;
    space->srv     = srv;
    space->h       = hash_create(buckets, value_dtor);
    assert(space->h);
    space->buckets = buckets;
    return space;
}

void kdb_space_destroy(kdb_space_t* space) {
    assert(space);
    hash_destroy(space->h);
    destroy(space->path);
    destroy(space);
}

int kdb_space_set_key(kdb_space_t* space, const char* path, const char* full_path, const void* value, int size, uint32_t flags, uint32_t exptime) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    assert(value);
    assert(size);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_set_key_path(space, path + i, full_path, name, value, size, flags, exptime);
    }
    return error;
}

int kdb_space_add_key(kdb_space_t* space, const char* path, const char* full_path, const void* value, int size, uint32_t flags, uint32_t exptime) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    assert(value);
    assert(size);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_add_key_path(space, path + i, full_path, name, value, size, flags, exptime);
    }
    return error;
}

int kdb_space_get_key(kdb_space_t* space, const char* path, kdb_space_value_t** value) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    assert(value);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_get_key_path(space, path + i, name, value);
    }
    return error;
}

int kdb_space_del_key(kdb_space_t* space, const char* path) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_del_key_path(space, path + i, name);
    }
    return error;
}

int kdb_space_update_key(kdb_space_t* space, const char* path, const void* value, int size, uint32_t flags, uint32_t exptime) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    assert(value);
    assert(size);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_update_key_path(space, path + i, name, value, size, flags, exptime, 0);
    }
    return error;
}

int kdb_space_cas_key(kdb_space_t* space, const char* path, const void* value, int size, uint32_t flags, uint32_t exptime, uint64_t cas_id) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    assert(value);
    assert(size);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_update_key_path(space, path + i, name, value, size, flags, exptime, cas_id);
    }
    return error;
}

int kdb_space_subscribe_key(kdb_space_t* space, const char* path, kchannel_ref_t* channel) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    assert(channel);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_subscribe_key_path(space, path + i, name, channel);
    }
    return error;
}

int kdb_space_forget_key(kdb_space_t* space, const char* path, kchannel_ref_t* channel) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    assert(channel);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_forget_key_path(space, path + i, name, channel);
    }
    return error;
}

int kdb_space_subscribe(kdb_space_t* space, const char* path, kchannel_ref_t* channel) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_subscribe_space_path(space, path + i, name, channel);
    }
    return error;
}

int kdb_space_forget(kdb_space_t* space, const char* path, kchannel_ref_t* channel) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_forget_space_path(space, path + i, name, channel);
    }
    return error;
}

int kdb_space_add_space(kdb_space_t* space, const char* path, const char* full_path, uint32_t exptime) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_add_space_path(space, path + i, full_path, name, exptime);
    }
    return error;
}

int kdb_space_get_space(kdb_space_t* space, const char* path, kdb_space_t** child) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    assert(child);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_get_space_path(space, path + i, name, child);
    }
    return error;
}

int kdb_space_del_space(kdb_space_t* space, const char* path) {
    int  i                       = 0;
    int  error                   = db_error_ok;
    char name[MAX_NAME_SIZE + 1] = {0};
    assert(space);
    assert(path);
    error = kdb_iterate_path(space, path, name, &i);
    if (db_error_ok == error) {
        error = kdb_space_del_space_path(space, path + i, name);
    }
    return error;
}

kdb_server_t* kdb_space_get_server(kdb_space_t* space) {
    return space->srv;
}

int kdb_space_get_buckets(kdb_space_t* space) {
    return space->buckets;
}

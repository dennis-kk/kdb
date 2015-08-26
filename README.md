# kdb
Key-value in memory object cache, support PUB/SUB event base on object, support most of memcached text protocol(not include `append` and `prepend` command).

# Space
The format of key of kdb like: `a.b.c`, a is the top level space, b is the second level space, c is the object.

# Subcribe/Publish
The new command of kdb support sub/pub mode, the command list below:   
1. sub  
2. subkey   
3. leave   
4. leavekey   

For example, there is a space `a.b.c` and several objects in space `a.b.c`: `a.b.c.c1`, `a.b.c.c2`. I subscribe the key: `subkey a.b.c.c2`, the PUB event occurs when someone run store command. I also can subscribe the whole space: `sub a.b.c` or `sub a`, the PUB event coming when any object in space has been changed.
`leavekey a.b.c.c2` cancel the PUB/SUB mode for the object, `leave a.b` cancel the PUB/SUB mode for the space.

# Plugin
User can write the plugin to hook the event in `kdb` process, the plugin is a common .dll or .so and expose some predefined API:

	/*! called after server stated */
	typedef int (*kdb_server_on_after_start_t)(kdb_server_t*);
	/*! called after server thread stopped, but the momery still can access */
	typedef int (*kdb_server_on_after_stop_t)(kdb_server_t*);
	/*! called when new space or object added  */
	typedef int (*kdb_server_on_key_after_add_t)(kdb_server_t*, kdb_space_value_t*);
	/*! called when object updated */
	typedef int (*kdb_server_on_key_after_update_t)(kdb_server_t*, kdb_space_value_t*);
	/*! called before delete object */
	typedef int (*kdb_server_on_key_before_delete_t)(kdb_server_t*, kdb_space_value_t*);
	/*! malloc */
	typedef void* (*kdb_server_malloc_t)(int);
	/*! realloc */
	typedef void* (*kdb_server_realloc_t)(void*, int);
	/*! free */
	typedef void (*kdb_server_free_t)(void*);

The expose API symbol name:

	 on_start
	 on_stop
	 on_add
	 on_update
	 on_delete
     on_malloc
     on_realloc
     on_free

# exptime
The `exptime` of memcached for now is not implemented, please set `exptime` to zero.

# PUB/SUB protocol

	addspace <key> <exptime>\r\n    New space
    deletespace <key>\r\n           Delete space(include all objects and child spaces)
	sub <key>\r\n                   Subscribe space
	subkey <key>\r\n                Subscribe object
	leave <key>\r\n                 Cancel space observing
	leavekey <key>\r\n              Cancel object observing

Possible acknowledge:

	STORED\r\n       success
	NOT_FOUND\r\n    <key> not found
	DELETED\r\n      space deleted

PUB event - object update notification:

	PUB UPDATED <key> <bytes>\r\n
	data\r\n
	END\r\n
	
PUB event - space deleting notification:

	PUB DELETED SPACE <key>\r\n

PUB event - object deleting notification:

	PUB DELETED OBJECT <key> <bytes>\r\n
	data\r\n
	END\r\n

PUB event - new space notification:

	PUB ADDED SPACE <key>\r\n

PUB event - new object notification:

	PUB ADDED OBJECT <key> <bytes>\r\n
	data\r\n
	END\r\n

# Build

## Dependency   
	knet http://github.com/dennis-kk/knet

## Linux   
	run build.sh

## Windows
	see /win-proj. Please set environment variable first:
		$(KNET_INCLUDE) knet include path,
		$(KNET_LIB)     knet library path.

# Future
1. Client
2. User defined hash function
3. Performance
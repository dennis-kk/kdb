# kdb
Key-value in memory object cache, support PUB/SUB event base on object, support most of memcached text protocol(not include `append` and `prepend` command).

# Space
The format of key of kdb like: `a.b.c`, a is the top level space, b is the second level space, c is the attribute.

# Subcribe/Publish
The new command of kdb support sub/pub mode, the command list below:   
1. sub  
2. subkey   
3. leave   
4. leavekey   

For example, there is a space `a.b.c` and several attributes in space `a.b.c`: `a.b.c.c1`, `a.b.c.c2`. I subscribe the key: `subkey a.b.c.c2`, the PUB event occurs when someone run store command. I also can subscribe the whole space: `sub a.b.c` or `sub a`, the PUB event coming when any attribute in space has been changed.
`leavekey a.b.c.c2` cancel the PUB/SUB mode for the attribute, `leave a.b` cancel the PUB/SUB mode for the space.

# Plugin
User can write the plugin to hook the event in `kdb` process, the plugin is a common .dll or .so and expose some predefined API:

	/*! called after server stated */
	typedef int (*kdb_server_on_after_start_t)(kdb_server_t*);
	/*! called after server thread stopped, but the momery still can access */
	typedef int (*kdb_server_on_after_stop_t)(kdb_server_t*);
	/*! called when new space or attribute added  */
	typedef int (*kdb_server_on_key_after_add_t)(kdb_server_t*, kdb_space_value_t*);
	/*! called when attribute updated */
	typedef int (*kdb_server_on_key_after_update_t)(kdb_server_t*, kdb_space_value_t*);
	/*! called before delete attribute */
	typedef int (*kdb_server_on_key_before_delete_t)(kdb_server_t*, kdb_space_value_t*);

The expose API symbol name:

	 on_start
	 on_stop
	 on_add
	 on_update
	 on_delete


# exptime
The `exptime` of memcached for now is not implemented, please set `exptime` to zero.

# Future
1. Client
2. User defined hash function
3. Protocol details of new commands
3. Performance
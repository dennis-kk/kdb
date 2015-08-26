#include "db_space.h"
#include "db_server.h"

int main(int argc, char** argv) {
    int error = db_error_ok;
    db_server = kdb_server_create();
    error = kdb_server_start(db_server, argc, argv);
    if (db_error_ok != error) {
        
    }
    kdb_server_wait_for_stop(db_server);
    kdb_server_destroy(db_server);
    return 0;
}

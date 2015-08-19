#include "db_space.h"
#include "db_server.h"

int main(int argc, char** argv) {
    db_server = kdb_server_create();
    kdb_server_start(db_server, argc, argv);
    kdb_server_wait_for_stop(db_server);
    kdb_server_destroy(db_server);
    return 0;
}

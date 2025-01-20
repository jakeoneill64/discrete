#include "sqlite3.h"

#ifndef DISCRETE_DATABASE_H
#define DISCRETE_DATABASE_H

// https://www.sqlite.org/threadsafe.html, defaults to serialised mode
// which is fine for us. We don't need great performance characteristics
// for our purposes.
inline sqlite3* get_sqlite_database() {
    static sqlite3* database;
    // TODO handle non-successful initialisation
    // TODO platform dependent path here.
    if(!database){
        sqlite3_open("discrete.db", &database);
    }
    return database;
}

#endif //DISCRETE_DATABASE_H

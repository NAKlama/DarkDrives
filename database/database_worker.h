//
// Created by Nina Alexandra Klama on 2026-10-03.
// Copyright (c) 2026 Nina Alexandra Klama
//


/*
 * This file is part of DarkDrives.
 * 
 * DarkDrives is free software: you can redistribute it and/or modify it under the terms of the GNU General Public 
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later 
 * version.
 * 
 * DarkDrives is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with Foobar. If not, see 
 * <https://www.gnu.org/licenses/>.
 */
#ifndef DARKDRIVES_DATABASE_WORKER_H
#define DARKDRIVES_DATABASE_WORKER_H

#include <filesystem>
#include <thread>

#include "sqlpp23/sqlite3/database/connection.h"


using namespace std;

class DatabaseWorker
{
public:
    DatabaseWorker(filesystem::path const & databasePath);
    ~DatabaseWorker();
private:
    void db_thread_worker();

    thread db_thread;
    sqlpp::sqlite3::connection db;
};


#endif //DARKDRIVES_DATABASE_WORKER_H

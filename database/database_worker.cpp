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
#include "database_worker.h"
#include <sqlite3.h>
#include "sqlpp23/sqlite3/database/connection_config.h"

DatabaseWorker::DatabaseWorker(filesystem::path const& databasePath)
{
    auto db_config = make_shared<sqlpp::sqlite3::connection_config>();
    db_config->path_to_database = databasePath;
    db_config->flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

    db.connect_using(db_config);
}

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
#include <list>
#include <string>
#include <optional>
#include <chrono>
#include <utility>

#include "sqlpp23/sqlite3/database/connection.h"
#include "db_config.h"


using namespace std;

import sqlpp23.core;
import sqlpp23.mysql;

import db_tables;

enum class inode_type
{
    DRIVE,
    DIR,
    FILE,
    LINK
};

typedef chrono::time_point<chrono::system_clock, chrono::microseconds> sql_timestamp;

struct inode_info
{
    uint64_t inode;
    optional<uint64_t> parent;
    string name;
    sql_timestamp c_time;
    sql_timestamp m_time;
};

struct stats
{
    uint64_t inode;
    uint64_t size;
    uint64_t blksize;
    uint32_t uid;
    uint32_t gid;
    sql_timestamp a_time;
    sql_timestamp m_time;
    sql_timestamp c_time;
    uint16_t mode;
    string mime_type;
};

struct exif_data
{

};

class DatabaseWorker
{
public:
    DatabaseWorker(const struct db_config& db_configuration);
    // ~DatabaseWorker();

    optional<inode_info> get_inode_info(uint64_t inode);
    list<inode_info>     get_child_info(uint64_t parent_inode);
    optional<inode_info> get_directory_inode(const string& path);



private:

    static list<string> split_path(const string& path);

    shared_ptr<sqlpp::mysql::connection_config> db_conf;

    sqlpp::mysql::connection db;
};


#endif //DARKDRIVES_DATABASE_WORKER_H

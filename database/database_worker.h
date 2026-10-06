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
#include "../cache.h"

#ifndef MIME_TYPE_CACHE_SIZE
#define MIME_TYPE_CACHE_SIZE 1024
#endif
#ifndef EXIF_FIELD_CACHE_SIZE
#define EXIF_FIELD_CACHE_SIZE 1024
#endif

using namespace std;

import sqlpp23.core;
import sqlpp23.mysql;

import db_tables;

enum class inode_type
{
    UNKNOWN,
    DIRECTORY,
    DRIVE_DIR,
    FILE,
    LINK
};

typedef chrono::time_point<chrono::system_clock, chrono::microseconds> sql_timestamp;

struct inode_info
{
    uint64_t            inode;
    optional<uint64_t>  parent;
    inode_type          type;
    string              name;
    sql_timestamp       c_time;
    sql_timestamp       m_time;
};

struct ins_inode_info
{
    optional<uint64_t>  parent;
    inode_type          type;
    string              name;
};

struct stats
{
    uint64_t        inode;
    uint64_t        file_size;
    uint64_t        blkcnt;
    uint32_t        uid;
    uint32_t        gid;
    uint32_t        blksize;
    sql_timestamp   a_time;
    sql_timestamp   m_time;
    sql_timestamp   c_time;
    uint16_t        mode;
    string          mime_type;
};

struct exif_data
{
    uint64_t inode;
    list<pair<string, string>> tags;
};

struct drive_info
{
    uint64_t drive_inode{};
    uint64_t disk_size{};
    uint64_t disk_free{};
    uint32_t drive_id{};
    optional<string> drive_uuid;
    optional<string> mount_point;
    bool autoscan{};
};

class DatabaseWorker
{
public:
    DatabaseWorker(const db_config& db_configuration,
        size_t mime_type_cache_size = MIME_TYPE_CACHE_SIZE,
        size_t exif_field_cache_size = EXIF_FIELD_CACHE_SIZE,
        char raw_id_prefix = '[',
        char raw_id_suffix = ']');
    // DatabaseWorker(const struct db_config& db_configuration);
    // ~DatabaseWorker();

    // SELECT Statements
    optional<inode_info>    get_inode_info(uint64_t inode);
    list<inode_info>        get_child_info(uint64_t parent_inode);
    optional<inode_info>    get_directory_inode_info(const string& path);

    optional<string>        get_mime_type_raw(uint32_t mime_type_id);
    string                  get_mime_type(uint32_t mime_type_id);
    optional<stats>         get_stats(uint64_t inode);
    optional<string>        get_file_output(uint64_t inode);
    optional<string>        get_exif_field_raw(uint16_t field_id);
    string                  get_exif_field(uint16_t field_id);
    exif_data               get_exif_data(uint64_t inode);

    optional<string>        get_drive_uuid(uint32_t drive_id);
    optional<string>        get_drive_mountpoint(uint32_t drive_id);
    optional<drive_info>    get_drive_info_by_inode(uint64_t inode);
    optional<drive_info>    get_drive_info_by_drive_id(uint32_t drive_id);

    // INSERT Statements
    bool                    add_new_inode(const ins_inode_info& info);
    bool                    mkdir(uint64_t parent, const string& name);

    // DELETE Statements
    bool                    remove_inode(uint64_t inode);

    // UPDATE Statements
    bool                    change_parent(uint64_t inode, uint64_t new_parent);

    // Complex Statements
    bool                    move_drive_by_inode(uint64_t drive_inode, uint64_t new_parent);
    bool                    move_drive_by_id(uint32_t drive_id, uint64_t new_parent);

protected:
    static inode_type          db2inode_type(const optional<string>& type);
    static optional<string>    inode_type2db(inode_type type);


private:
    char raw_id_prefix_, raw_id_suffix_;
    Cache<uint32_t, optional<string>> mime_type_cache_;
    Cache<uint32_t, optional<string>> exif_field_cache_;

    shared_ptr<sqlpp::mysql::connection_config> db_conf;

    sqlpp::mysql::connection db;
};


#endif //DARKDRIVES_DATABASE_WORKER_H

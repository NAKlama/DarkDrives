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

#include "../string_functions.h"


constexpr auto t_inode = db_t::t_inode{};
constexpr auto t_stats = db_t::t_stats{};
constexpr auto t_mime_type = db_t::t_mime_type{};
constexpr auto t_file_output = db_t::t_file_output{};
constexpr auto t_exif_field = db_t::t_exif_field{};
constexpr auto t_exif_data = db_t::t_exif_data{};

DatabaseWorker::DatabaseWorker(const struct db_config& db_configuration)
{
    // auto db_config = make_shared<sqlpp::sqlite3::connection_config>();
    // db_config->path_to_database = databasePath;
    // db_config->flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

    sqlpp::mysql::global_library_init();

    db_conf = make_shared<sqlpp::mysql::connection_config>();
    if (! db_configuration.host.empty())
    {
        db_conf->host = db_configuration.host;
        if (db_configuration.port != 0) db_conf->port = db_configuration.port;
        else db_conf->port = 3306;
    }
    db_conf->user = db_configuration.username;
    db_conf->password = db_configuration.password;
    db.connect_using(db_conf);

}

optional<inode_info> DatabaseWorker::get_inode_info(uint64_t inode_in)
{
    inode_info info = {};
    auto result_table = db(
        select(t_inode.id, t_inode.parent, t_inode.node_name, t_inode.type, t_inode.c_time, t_inode.m_time)
        .from(t_inode)
        .where(t_inode.id == inode_in));
    if (result_table.empty())
        return {};
    const auto& row = result_table.front();
    info = {
        .inode  = row.id,
        .parent = row.parent,
        .name   = string(row.node_name),
        .c_time = row.c_time,
        .m_time = row.m_time
    };
    return {info};
}

list<inode_info> DatabaseWorker::get_child_info(uint64_t parent_inode)
{
    list<inode_info> child_info_list = {};
    auto result_table = db(
        select(t_inode.id, t_inode.parent, t_inode.node_name, t_inode.type, t_inode.c_time, t_inode.m_time)
        .from(t_inode)
        .where(t_inode.parent == parent_inode));
    if (result_table.empty())
        return {};
    for (const auto& row : result_table)
    {
        inode_info info = {
           .inode  = row.id,
           .parent = row.parent,
           .name   = string(row.node_name),
           .c_time = row.c_time,
           .m_time = row.m_time
        };
        child_info_list.push_back(info);
    }
    return {child_info_list};
}

optional<inode_info> DatabaseWorker::get_directory_inode(const string& path)
{
    uint64_t inode = 0;
    inode_info info;
    inode_info child_info = {};
    list<string> path_dirs = str_f::split_delimiter('/', path);
    if (path_dirs.empty()) return {0};
    list<inode_info> children = get_child_info(0);
    if (children.empty()) return {};
    for (const auto& path_dir : path_dirs)
    {
        uint64_t child_inode = 0;
        for (const auto& child : children)
        {
            if (child.name == path_dir)
            {
                child_inode = child.inode;
                child_info = {child};
            }
        }
        if (child_inode != 0)
        {
            children = get_child_info(child_inode);
        }
    }
    return {child_info};
}



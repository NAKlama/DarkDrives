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
constexpr auto t_drives = db_t::t_drives{};
constexpr auto t_drive_uuid = db_t::t_drive_uuid{};
constexpr auto t_drive_mountpoint = db_t::t_drive_mountpoint{};

DatabaseWorker::DatabaseWorker(
    const db_config& db_configuration,
    const size_t mime_type_cache_size,
    const size_t exif_field_cache_size,
    char raw_id_prefix, char raw_id_suffix) :
        raw_id_prefix_(raw_id_prefix),
        raw_id_suffix_(raw_id_suffix),
        mime_type_cache_(Cache<uint32_t, optional<string>>(
            [this](const uint32_t x) -> optional<string>
            {
                return get_mime_type_raw(x);
            }, mime_type_cache_size)),
        exif_field_cache_(Cache<uint32_t, optional<string>>(
            [this](const uint32_t x) -> optional<string>
            {
                return get_exif_field_raw(x);
            }, exif_field_cache_size))
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

optional<string> DatabaseWorker::get_mime_type_raw(const uint32_t mime_type_id)
{
    const auto result_table = db(
        select(t_mime_type.id, t_mime_type.mime_type)
        .from(t_mime_type)
        .where(t_mime_type.id == mime_type_id));
    if (result_table.empty())
        return {};
    return string(result_table.front().mime_type);
}

string DatabaseWorker::get_mime_type(const uint32_t mime_type_id)
{
    optional<string> res = mime_type_cache_.get(mime_type_id);
    if (! res.has_value())
    {
        stringstream outstring;
        outstring << "[" << mime_type_id << "]";
        return outstring.str();
    }
    return string(res.value());
}

optional<stats> DatabaseWorker::get_stats(const uint64_t inode)
{
    const auto result_table = db(
        select(t_stats.inode, t_stats.file_size, t_stats.blkcnt, t_stats.uid, t_stats.gid, t_stats.blksize,
            t_stats.a_time, t_stats.m_time, t_stats.c_time, t_stats.mode, t_stats.mime_type)
        .from(t_stats)
        .where(t_stats.inode == inode));
    if (result_table.empty())
        return {};
    const auto& row = result_table.front();
    stats out = {
        .inode      = inode,
        .file_size  = row.file_size,
        .blkcnt     = row.blkcnt,
        .uid        = static_cast<uint32_t>(row.uid),
        .gid        = static_cast<uint32_t>(row.gid),
        .blksize    = static_cast<uint32_t>(row.blksize),
        .a_time     = row.a_time,
        .m_time     = row.m_time,
        .c_time     = row.c_time,
        .mode       = static_cast<uint16_t>(row.mode),
        .mime_type  = get_mime_type(row.mime_type)
    };
    return out;
}

optional<string> DatabaseWorker::get_file_output(const uint64_t inode)
{
    const auto result_table = db(
        select(t_file_output.inode, t_file_output.file_output)
        .from(t_file_output)
        .where(t_file_output.inode == inode));
    if (result_table.empty())
        return {};
    const auto& row = result_table.front();
    return {string(row.file_output)};
}

optional<string> DatabaseWorker::get_exif_field_raw(const uint16_t field_id)
{
    const auto result_table = db(
        select(t_exif_field.field_id, t_exif_field.field_name)
        .from(t_exif_field)
        .where(t_exif_field.field_id == field_id));
    if (result_table.empty())
        return {};
    const auto& row = result_table.front();
    return {string(row.field_name)};
}

string DatabaseWorker::get_exif_field(const uint16_t field_id)
{
    optional<string> res = exif_field_cache_.get(field_id);
    if (! res.has_value())
    {
        stringstream outstring;
        outstring << "[" << field_id << "]";
        return outstring.str();
    }
    return string(res.value());
}

exif_data DatabaseWorker::get_exif_data(const uint64_t inode) // NOLINT(*-convert-member-functions-to-static)
{
    exif_data out = {
        .inode = inode,
        .tags  = {}
    };
    for (const auto& row : db(
        select(t_exif_data.inode, t_exif_data.exif_field, t_exif_data.value)
        .from(t_exif_data)
        .where(t_exif_data.inode == inode)))
    {
        out.tags.push_back(
            make_pair(
                get_exif_field(row.exif_field),
                string(row.value)));
    }
    return out;
}

optional<string> DatabaseWorker::get_drive_uuid(uint32_t drive_id)
{
    const auto result_table = db(
        select(all_of(t_drive_uuid))
        .from(t_drive_uuid)
        .where(t_drive_uuid.drive_id == drive_id));
    if (result_table.empty()) return {};
    auto& row = result_table.front();
    if (row.disk_uuid.has_value())
        return string(row.disk_uuid.value());
    return {};
}

optional<string> DatabaseWorker::get_drive_mountpoint(uint32_t drive_id)
{
    const auto result_table = db(
     select(all_of(t_drive_mountpoint))
     .from(t_drive_mountpoint)
     .where(t_drive_mountpoint.drive_id == drive_id));
    if (result_table.empty()) return {};
    auto& row = result_table.front();
    return string(row.mountpoint);
}

optional<drive_info> DatabaseWorker::get_drive_info_by_inode(const uint64_t inode)
{
    const auto result_table = db(
        select(all_of(t_drives))
        .from(t_drives)
        .where(t_drives.inode == inode));
    if (result_table.empty())
        return {};
    const auto& row = result_table.front();

    drive_info out = {
        .drive_inode = row.inode,
        .disk_size   = row.disk_size,
        .disk_free   = row.disk_free,
        .drive_id    = static_cast<uint32_t>(row.drive_id),
        .drive_uuid  = get_drive_uuid(row.drive_id),
        .mount_point = get_drive_mountpoint(row.drive_id)
    };
    return {out};
}

optional<drive_info> DatabaseWorker::get_drive_info_by_drive_id(uint32_t drive_id)
{
    const auto result_table = db(
    select(all_of(t_drives))
    .from(t_drives)
    .where(t_drives.drive_id == drive_id));
    if (result_table.empty())
        return {};
    const auto& row = result_table.front();

    drive_info out = {
        .drive_inode = row.inode,
        .disk_size   = row.disk_size,
        .disk_free   = row.disk_free,
        .drive_id    = static_cast<uint32_t>(row.drive_id),
        .drive_uuid  = get_drive_uuid(row.drive_id),
        .mount_point = get_drive_mountpoint(row.drive_id)
    };
    return {out};
}

bool DatabaseWorker::add_new_inode(const ins_inode_info& info)
{
    try
    {
        string type;

        db(insert_into(t_inode).set(
            t_inode.parent = info.parent,
            t_inode.type = inode_type2db(info.type),
            t_inode.node_name = info.name,
            t_inode.c_time = chrono::system_clock::now(),
            t_inode.m_time = chrono::system_clock::now()
            ));
    } catch (const sqlpp::mysql::exception& e)
    { return false; }
    return true;
}

bool DatabaseWorker::mkdir(const uint64_t parent, const string& name)
{
    const ins_inode_info new_inode = {
        .parent = parent,
        .type = inode_type::DIRECTORY,
        .name = name,
    };
    return add_new_inode(new_inode);
}

bool DatabaseWorker::remove_inode(const uint64_t inode)
{
    optional<inode_info> to_remove_info = get_inode_info(inode);
    if (! to_remove_info.has_value()) return false;
    if (to_remove_info.value().type != inode_type::DIRECTORY) return false;

    list<inode_info> children = get_child_info(inode);
    if (! children.empty()) return false;

    try
    {
        db(delete_from(t_inode).where(t_inode.id == inode));
    } catch (const sqlpp::mysql::exception& e)
    { return false ;}
    return true;
}

bool DatabaseWorker::change_parent(const uint64_t inode, const uint64_t new_parent)
{
    if (inode == 0) return false;
    const optional<inode_info> inode_data = get_inode_info(inode);
    if (! inode_data.has_value()) return false;
    if (! (inode_data.value().type == inode_type::DIRECTORY || inode_data.value().type == inode_type::DRIVE_DIR))
        return false;
    if (inode_data.value().type == inode_type::DRIVE_DIR && inode_data.value().parent.has_value())
    {
        const optional<inode_info> parent = get_inode_info(inode_data.value().parent.value());
        if (! parent.has_value()) return false;
        if (parent.value().type != inode_type::DIRECTORY) return false;
    }

    const optional<inode_info> new_parent_data = get_inode_info(new_parent);
    if (! new_parent_data.has_value()) return false;
    if (new_parent_data.value().type != inode_type::DIRECTORY) return false;

    optional<inode_info> ptr = new_parent_data;

    while (ptr.value().parent != 0)
    {
        if (ptr.value().parent == inode) return false;
        ptr = get_inode_info(ptr.value().parent.value());
        if (! ptr.has_value()) return false;
        if (! ptr.value().parent.has_value()) return false;
    }

    try
    {
        db(update(t_inode)
            .set(t_inode.parent = new_parent)
            .where(t_inode.id == inode));
    } catch (const sqlpp::mysql::exception& e) { return false; }
    return true;
}

bool DatabaseWorker::move_drive_by_inode(uint64_t drive_inode, uint64_t new_parent)
{
    return change_parent(drive_inode, new_parent);
}

bool DatabaseWorker::move_drive_by_id(uint32_t drive_id, uint64_t new_parent)
{
    optional<drive_info> drive_info = get_drive_info_by_drive_id(drive_id);
    if (! drive_info.has_value()) return false;
    return change_parent(drive_info.value().drive_inode, new_parent);
}

inode_type DatabaseWorker::db2inode_type(const optional<string>& type)
{
    if (! type.has_value())
        return inode_type::UNKNOWN;
    if (type.value() == "DIRECTORY") return inode_type::DIRECTORY;
    if (type.value() == "DRIVE_DIR") return inode_type::DRIVE_DIR;
    if (type.value() == "FILE")      return inode_type::FILE;
    if (type.value() == "LINK")      return inode_type::LINK;
    return inode_type::UNKNOWN;
}

optional<string> DatabaseWorker::inode_type2db(const inode_type type)
{
    switch (type)
    {
        case inode_type::DIRECTORY: return "DIRECTORY";
        case inode_type::DRIVE_DIR: return "DRIVE_DIR";
        case inode_type::FILE:    return "FILE";
        case inode_type::LINK:    return "LINK";
        default: return {};
    }
}


optional<inode_info> DatabaseWorker::get_inode_info(const uint64_t inode_in)
{
    inode_info info = {};
    const auto result_table = db(
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

list<inode_info> DatabaseWorker::get_child_info(const uint64_t parent_inode)
{
    list<inode_info> child_info_list = {};
    for (const auto& row : db(
            select(t_inode.id, t_inode.parent, t_inode.node_name, t_inode.type, t_inode.c_time, t_inode.m_time)
            .from(t_inode)
            .where(t_inode.parent == parent_inode)))
    {
        string inode_type_str;
        const inode_type type = db2inode_type(string(row.type.value()));

        inode_info info = {
           .inode  = row.id,
           .parent = row.parent,
           .type   = type,
           .name   = string(row.node_name),
           .c_time = row.c_time,
           .m_time = row.m_time
        };
        child_info_list.push_back(info);
    }
    return {child_info_list};
}

optional<inode_info> DatabaseWorker::get_directory_inode_info(const string& path)
{
    uint64_t inode = 0;
    inode_info info;
    inode_info child_info = {};
    list<string> path_dirs = str_f::split_delimiter('/', path);
    if (path_dirs.empty()) return get_inode_info(0);
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



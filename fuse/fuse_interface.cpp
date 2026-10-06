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

#include "fuse_interface.h"


#include <cassert>
#include <memory>
#include <iostream>
#include <sstream>
#include <limits>

#include "../string_functions.h"
#include "../int_functions.h"



static Config *g_conf = nullptr;
static DatabaseWorker *g_db_worker = nullptr;
static fuse_operations operations = {};
static user_ids g_mount_ids;
static map<uint64_t, OpenFile> g_open_files = {};
static auto g_control_buffer = stringstream();
static bool g_control_buffer_open = false;

static void *fuse_init(struct fuse_conn_info *conn) {
    assert(g_conf != nullptr);
    g_db_worker = g_conf->create_db_worker().get();
    return nullptr;
};

static int fuse_getattr(const char *c_path, struct stat *st)
{
    int res = 0;
    std::string path(c_path);
    memset(st, 0, sizeof(struct stat));
    if (path == "/")
    {
        st->st_mode = S_IFDIR | 0775;
        st->st_uid = g_mount_ids.uid;
        st->st_gid = g_mount_ids.gid;
        st->st_nlink = 2;
        return 0;
    }
    if (path == "/.control")
    {
        st->st_mode = S_IFREG | 0600;
        st->st_uid = g_mount_ids.uid;
        st->st_gid = g_mount_ids.gid;
        st->st_nlink = 2;
        return 0;
    }
    auto inode_info = g_db_worker->get_directory_inode_info(path);
    if ( ! inode_info.has_value() || inode_info.value().type == inode_type::UNKNOWN)
    {
        return -ENOENT;
    }
    if (inode_info.value().type == inode_type::DIRECTORY || inode_info.value().type == inode_type::DRIVE_DIR)
    {
        if (inode_info.value().type == inode_type::DRIVE_DIR)
            st->st_mode = S_IFDIR | 0555;
        else
            st->st_mode = S_IFDIR | 0775;
        st->st_uid = g_mount_ids.uid;
        st->st_gid = g_mount_ids.gid;
        st->st_nlink = 2;  // Hopefully we won't have to count these
        return 0;
    }
    if (inode_info.value().type == inode_type::FILE)
    {
        st->st_mode = S_IFREG | 0444;
        st->st_uid = g_mount_ids.uid;
        st->st_gid = g_mount_ids.gid;
        st->st_nlink = 1;
        return 0;
    }
    return 0;
}

static int fuse_readdir(const char* path, void* buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)
{
    filler(buffer, ".", nullptr, 0);
    filler(buffer, "..", nullptr, 0);
    if (strcmp(path, "/") == 0)
    {
        filler(buffer, ".control", nullptr, 0);
    }
    optional<inode_info> inode_info = g_db_worker->get_directory_inode_info(path);
    if (!inode_info.has_value())
        return -ENOENT;
    if (!inode_info.value().parent.has_value())
        return -ENOENT;
    for (auto& entry : g_db_worker->get_child_info(inode_info.value().parent.value()))
    {
        filler(buffer, entry.name.c_str(), nullptr, 0);
    }
    return 0;
}

static optional<OpenFile> create_new_file_buffer(uint64_t inode)
{
    using str_f::decode_mode_bits;

    if (optional<inode_info> inode_info = g_db_worker->get_inode_info(inode); !inode_info.has_value())
        return {};

    YAML::Emitter out;
    out << YAML::BeginMap; // Top level map of metadata types

    // Stats
    if (optional<stats> stats = g_db_worker->get_stats(inode); stats.has_value())
    {
        out << YAML::Key << "stats";
        out << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "size"      << YAML::Value << stats.value().file_size;
        out << YAML::Key << "blkcnt"    << YAML::Value << stats.value().blkcnt;
        out << YAML::Key << "blksize"   << YAML::Value << stats.value().blksize;
        out << YAML::Key << "uid"       << YAML::Value << stats.value().uid;
        out << YAML::Key << "gid"       << YAML::Value << stats.value().gid;
        out << YAML::Key << "a_time"    << YAML::Value << format("{:%F %T}", stats.value().a_time);
        out << YAML::Key << "m_time"    << YAML::Value << format("{:%F %T}", stats.value().m_time);
        out << YAML::Key << "c_time"    << YAML::Value << format("{:%F %T}", stats.value().c_time);
        out << YAML::Key << "mode"      << YAML::Value << decode_mode_bits(static_cast<uint16_t>(stats.value().mode));
        out << YAML::Key << "mime-type" << YAML::Value << stats.value().mime_type;
        out << YAML::EndMap;
    }

    // `file` Output
    if (optional<string> file_output = g_db_worker->get_file_output(inode); file_output.has_value())
    {
        out << YAML::Key << "file";
        out << YAML::Value << file_output.value();
    }

    // EXIF Data
    if (exif_data exif; ! exif.tags.empty())
    {
        out << YAML::Key << "exif";
        out << YAML::Value << YAML::BeginMap;
        for (auto& tag : exif.tags)
        {
            auto [key, value] = tag;
            out << YAML::Key << key;
            out << YAML::Value << value;
        }
        out << YAML::EndMap;
    }
    out << YAML::EndMap;   // Top level map of metadata types
    return {OpenFile(make_unique<string>(out.c_str()))};
}

static int fuse_open(const char* path, struct fuse_file_info* fi)
{
    if (strcmp(path, "/.control") == 0)
    {
        if (fi->flags & O_RDONLY)
        {
            return -EACCES;
        }
        if (g_control_buffer_open)
        {
            return -EPERM;
        }
        g_control_buffer_open = true;
        g_control_buffer.str("");
        fi->fh = 0;
    }
    const optional<inode_info> inode_info = g_db_worker->get_directory_inode_info(path);
    if (!inode_info.has_value())
    {
        return -ENOENT;
    }
    if (fi->flags & O_WRONLY)
    {
        return -EACCES;
    }
    uint64_t inode = inode_info.value().inode;
    optional<OpenFile> new_OpenFile = create_new_file_buffer(inode);
    if (!new_OpenFile.has_value())
    {
        return -ENOENT;
    }
    g_open_files.insert(make_pair(inode, move(new_OpenFile.value())));
    fi->fh = inode;
    return 0;
}

// static int fuse_flush(const char* path, struct fuse_file_info* fi)
// {
//     return 0;
// }

static int fuse_release(const char* path, struct fuse_file_info* fi)
{
    const uint64_t inode = fi->fh;
    if (inode == 0) // file handle for /.control
    {
        // TODO: implement control interface
        g_control_buffer_open = false;
    }
    for (auto it = g_open_files.begin(); it != g_open_files.end(); ++it)
    {
        if (it->first == inode)
        {
            g_open_files.erase(it);
        }
    }
    return 0;
}

static int fuse_read(
    const char* path,
    char* buffer,
    const size_t size_in,
    const off_t offset,
    struct fuse_file_info* fi)
{
    size_t size = size_in;

    if (size > numeric_limits<int>::max())
        size = numeric_limits<int>::max();

    // for /.control
    if ( fi->fh == 0 )
    {
        if (offset > g_control_buffer.str().size()) return 0;

        const string data = g_control_buffer.str().substr(offset, size);
        const size_t data_size = data.size();
        strncpy(buffer, data.c_str(), data_size);
        return static_cast<int>(data_size);
    }

    const OpenFile* of = nullptr;
    for (auto& [fd, open_file] : g_open_files)
    {
        if (fd == fi->fh)
        {
            of = &open_file;
        }
    }
    if (of == nullptr) return -EBADF;
    if (offset > of->size()) return 0;

    auto data = of->read(offset, size);
    size_t data_size = data.size();
    strncpy(buffer, data.data(), data_size);
    return static_cast<int>(data_size);
}

static int fuse_write(
    const char* path,
    const char* buffer,
    const size_t size_in,
    const off_t offset,
    struct fuse_file_info* fi)
{
    size_t size = size_in;

    if (size > numeric_limits<int>::max())
        size = numeric_limits<int>::max();

    if (fi->fh != 0) return -EACCES;
    auto new_data = string(buffer, size);
    const size_t buffer_size = g_control_buffer.str().size();
    if (buffer_size < offset) {
        const streamsize fill_cnt = int_f::cast_to_signed(offset - buffer_size);
        auto filler = string(fill_cnt, '\0');
        g_control_buffer.seekp(0, std::ios_base::end);
        g_control_buffer.write(filler.c_str(), fill_cnt);
    };
    g_control_buffer.seekp(offset, std::ios_base::beg);
    g_control_buffer.write(new_data.c_str(), int_f::cast_to_signed(new_data.size()));
    return static_cast<int>(size);
}

static int fuse_mkdir(const char* path, mode_t mode)
{
    auto path_segments = str_f::split_delimiter('/', path);
    string new_dir_name = path_segments.back();
    path_segments.pop_back();

    const optional<inode_info> parent_dir = g_db_worker->get_directory_inode_info(str_f::join('/', path_segments));
    if (!parent_dir.has_value()) return -ENOENT;
    if (new_dir_name.size() > 255) return -ENAMETOOLONG;
    if (parent_dir.value().type != inode_type::DIRECTORY) return -EACCES;

    for (auto& inode_info : g_db_worker->get_child_info(parent_dir.value().inode))
    {
        if (inode_info.name == new_dir_name) return -EEXIST;
    }

    if (! g_db_worker->mkdir(parent_dir.value().inode, new_dir_name)) return -EACCES;
    return 0;
}

static int fuse_rmdir(const char* path)
{
    optional<inode_info> inode_data = g_db_worker->get_directory_inode_info(path);
    if (! inode_data.has_value()) return -ENOENT;
    if (inode_data.value().type != inode_type::DIRECTORY) return -ENOTDIR;
    list<inode_info> children = g_db_worker->get_child_info(inode_data.value().inode);
    if (!children.empty()) return -ENOTEMPTY;

    if (! g_db_worker->remove_inode(inode_data.value().inode)) return -EACCES;
    return 0;
}

static int fuse_rename(const char* oldpath, const char* newpath)
{
    optional<inode_info> inode_data = g_db_worker->get_directory_inode_info(oldpath);
    if (! inode_data.has_value()) return -ENOENT;
    if (! (inode_data.value().type == inode_type::DIRECTORY || inode_data.value().type == inode_type::DRIVE_DIR))
        return -EACCES;

    optional<inode_info> target_data = g_db_worker->get_directory_inode_info(newpath);
    if (! inode_data.has_value()) return -ENOENT;
    if (inode_data.value().type != inode_type::DIRECTORY) return -EACCES;

    list<inode_info> target_children = g_db_worker->get_child_info(target_data.value().inode);
    for (auto& child_info : target_children)
    {
        if (child_info.name == inode_data.value().name) return -EEXIST;
    }

    optional<inode_info> parent_data;
    if (! inode_data.value().parent.has_value())
        parent_data = {};
    else
        parent_data = g_db_worker->get_inode_info(inode_data.value().parent.value());
    if (inode_data.value().type != inode_type::DRIVE_DIR && parent_data.value().type != inode_type::DIRECTORY)
        return -EACCES;

    list<string> old_path_segments = str_f::split_delimiter('/', oldpath);
    list<string> new_path_segments = str_f::split_delimiter('/', newpath);

    bool same = true;

    auto iter_old = old_path_segments.begin();
    auto iter_new = new_path_segments.begin();
    if ( new_path_segments.size() > old_path_segments.size() )
        for (; iter_old != old_path_segments.end() && iter_new != old_path_segments.end(); ++iter_old, ++iter_new)
            if (*iter_old != *iter_new) same = false;
    if (same) return -EACCES;

    return g_db_worker->change_parent(inode_data.value().inode, parent_data.value().inode);
}

int fuse_interface::start_fuse(unique_ptr<Config> conf, const std::filesystem::path& mountpoint, struct user_ids& uids)
{
    if (conf == nullptr)
    {
        throw std::runtime_error("init_fuse_interface: Config *conf == nullptr");
    }
    g_conf = conf.get();
    operations = {
        .getattr = fuse_getattr,
        .mkdir   = fuse_mkdir,
        .rmdir   = fuse_rmdir,
        .rename  = fuse_rename,
        .open    = fuse_open,
        .read    = fuse_read,
        .write   = fuse_write,
        // .flush   = fuse_flush,
        .release = fuse_release,
        .readdir = fuse_readdir,
        .init    = fuse_init,
    };
    const int argc = 1;
    auto argv = new char*[argc + 1];
    string absolute_mountpoint = std::filesystem::absolute(mountpoint).string();
    argv[0] = new char[absolute_mountpoint.size() + 1];
    strncpy(argv[0], absolute_mountpoint.c_str(), absolute_mountpoint.size() + 1);
    argv[0][absolute_mountpoint.size()] = '\0';

    // struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    // if (fuse_opt_parse(&args, nullptr, option_spec, nullptr) == -1)
    //     exit(1);
    int ret = fuse_main(argc, argv, &operations, nullptr);
    return ret;
}


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

static Config *g_conf = nullptr;
static DatabaseWorker *g_db_worker = nullptr;
static fuse_operations operations = {};
static user_ids g_mount_ids;

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
    }
}

// static const struct fuse_opt option_spec[] = {FUSE_OPT_END };

int fuse_interface::start_fuse(unique_ptr<Config> conf, const std::filesystem::path& mountpoint, struct user_ids& uids)
{
    if (conf == nullptr)
    {
        throw std::runtime_error("init_fuse_interface: Config *conf == nullptr");
    }
    g_conf = conf.get();
    operations = {
        .getattr = fuse_getattr,
        .init = fuse_init,
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


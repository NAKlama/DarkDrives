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

static Config *g_conf = nullptr;

static void *fuse_init(struct fuse_conn_info *conn) {
    assert(g_conf != nullptr);
    g_conf->create_db_worker();
    return nullptr;
};

FuseInterface::FuseInterface(Config *conf)
{
    if (conf == nullptr)
    {
        throw std::runtime_error("FuseInterface::FuseInterface: Config* == nullptr");
    }
    g_conf = conf;
    operations = {
        .init = fuse_init,
    };
}

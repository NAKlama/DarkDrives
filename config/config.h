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
#ifndef DARKDRIVES_CONFIG_H
#define DARKDRIVES_CONFIG_H

#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "../database/database_worker.h"



class Config
{
    Config(filesystem::path config_file_ = filesystem::path());
    ~Config();
public:
    void create_db_worker();
    DatabaseWorker *db_worker = nullptr;

private:
    filesystem::path config_file;
    YAML::Node config;
    filesystem::path data_dir;
    filesystem::path database;
};


#endif //DARKDRIVES_CONFIG_H

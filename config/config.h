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
#include <list>



#include "../database/database_worker.h"
#include "../database/db_config.h"
#include "../logger.h"



class Config
{
public:
    Config(Logger logger, filesystem::path config_file_ = filesystem::path());
    // ~Config();

    unique_ptr<DatabaseWorker> create_db_worker();
    bool get_logging_enabled() const;
    std::shared_ptr<spdlog::logger> get_logger() const;
private:
    Logger logger;
    filesystem::path config_file;
    YAML::Node config;
    struct db_config db_conf;

    static string YAML_check_keyword(YAML::Node node, const list<string>& keywords);
};


#endif //DARKDRIVES_CONFIG_H

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

#include "config.h"

#include <list>
#include <utility>

#include "exceptions.h"
#include "../database/database_worker.h"

using namespace std;


string Config::YAML_check_keyword(YAML::Node node, const list<string>& keywords) {
    for (auto &key : keywords)
    {
        if (node[key]) return key;
    }
    return string();
}

Config::Config(Logger logger_in, filesystem::path config_file_) : config_file(std::move(config_file_)),
                                                                  logger(std::move(logger_in))
{
    static list<filesystem::path> default_config_paths = {
        "/etc/DarkDrives.yaml",
        "~/.DarkDrives.yaml",
        "~/.config/DarkDrives/config.yaml",
    };
    if (config_file.empty())
    {
        for (auto &file : default_config_paths)
        {
            if (filesystem::exists(file))
            {
                config_file = file;
                break;
            }
        }
    }
    if (! config_file.empty())
    {
        config_file = filesystem::absolute(config_file);
        config = YAML::LoadFile(config_file);
        if (config.Type() != YAML::NodeType::Map) throw ExceptionConfigRootNotMap();
        auto db_key = YAML_check_keyword(config,
            {"mysql", "MySQL", "mariadb", "MariaDB"});
        if (db_key.empty()) throw ExceptionNoDatabaseConfigured();
        if (config[db_key].Type() != YAML::NodeType::Map)
            throw ExceptionConfigDatatype(db_key + ": Datatype is not map!");
        auto mysql_conf = config[db_key];
        if (mysql_conf["host"]) db_conf.host = mysql_conf["host"].as<string>();
        else db_conf.host = string();
        if (mysql_conf["port"]) db_conf.port = mysql_conf["port"].as<int>();
        else db_conf.port = 0;
        auto user_kw = YAML_check_keyword(mysql_conf, {"user", "username"});
        if (user_kw.empty()) throw ExceptionConfigMissingData("mysql: user: MySQL username missing!");
        db_conf.username = mysql_conf[user_kw].as<string>();
        auto pass_kw = YAML_check_keyword(mysql_conf, {"pass", "password"});
        if (pass_kw.empty()) throw ExceptionConfigMissingData("mysql: pass: MySQL password missing!");
        db_conf.password = mysql_conf[pass_kw].as<string>();
    }
    else
        throw ExceptionNoConfigurationFound();
}


unique_ptr<DatabaseWorker> Config::create_db_worker()
{
    return make_unique<DatabaseWorker>(db_conf);
}

bool Config::get_logging_enabled() const
{
    return logger.active;
}

std::shared_ptr<spdlog::logger> Config::get_logger() const
{
    return logger.logger;
}

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


Config::Config(filesystem::path config_file_) : config_file(std::move(config_file_))
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
        if (config["data_dir"])
        {
            data_dir = config["data_dir"].as<std::string>();
        } else
        {
            throw ExceptionNoDataDirConfigured();
        }
    } else
    {
        config_file = filesystem::absolute(default_config_paths.back());
        filesystem::create_directories(config_file.parent_path());
        YAML::Emitter out;
        out.SetOutputCharset(YAML::EscapeNonAscii);
        out << YAML::BeginMap;
        out << YAML::Key << "data_dir";
        out << YAML::Value << "/etc/DarkDrives";
        out << YAML::EndMap;
    }
    database = filesystem::absolute(data_dir) / "DarkDrives.sqlite";
}

void Config::create_db_worker()
{
    db_worker = new DatabaseWorker(database);
}

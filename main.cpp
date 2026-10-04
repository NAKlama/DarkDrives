//
// Created by Nina Alexandra Klama on 2026-10-03.
// Copyright (c) 2026 Nina Alexandra Klama
//

/*
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program. If not, see
 * <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <tclap/CmdLine.h>
#include <string>

#include "pwd.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "logger.h"
#include "config/config.h"
#include "fuse/fuse_interface.h"


static auto version = "0.1";

static user_ids get_user_ids(const std::string& username)
{
    struct passwd *pwd;
    struct user_ids ids = {0, 0};
    pwd = getpwnam(username.c_str());
    ids.uid = pwd->pw_uid;
    ids.gid = pwd->pw_gid;
    free(pwd);
    return ids;
}

int main(int argc, char** argv)
{
    Logger log;
    unique_ptr<Config> config;
    try
    {
        TCLAP::CmdLine cmd("DarkDrives", ' ', version);
        TCLAP::ValueArg<std::string> config_file_arg("c", "conf", "Config file", false, "", "config file");
        TCLAP::ValueArg<std::string> log_file_arg("l", "log", "Log file", false, "", "log file");
        TCLAP::ValueArg<std::string> mount_arg("m", "mount", "Mount filesystem", false, "", "mountpouint");
        TCLAP::ValueArg<std::string> owner_arg("u", "user", "User for filesystem owner", false, "", "filesystem owner");
        cmd.add(config_file_arg);
        cmd.add(log_file_arg);
        cmd.add(mount_arg);
        cmd.add(owner_arg);
        cmd.parse(argc, argv);

        std::string config_file = log_file_arg.getValue();

        if (std::string log_file = log_file_arg.getValue(); !log_file.empty())
        {
            log.active = true;
            log.logger = spdlog::basic_logger_mt("file_logger", log_file);
            spdlog::set_default_logger(log.logger);
            spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%F %z} [%=8l] [P:%P T:%t] [%@] %^%v%$");
        } else
        {
            log.active = false;
        }

        auto mount_point = std::filesystem::path(mount_arg.getValue());
        auto user_ids = get_user_ids(config_file_arg.getValue());

        if (!mount_point.empty())
            return fuse_interface::start_fuse(make_unique<Config>(log, config_file), mount_point, user_ids);

    } catch (TCLAP::ArgException &e)
    {
        std::cerr << "Arg Exception: " << e.error() << " for arg " << e.argId() << std::endl;
    }
    return 0;
}
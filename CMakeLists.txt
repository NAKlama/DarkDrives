#
# Created by Nina Alexandra Klama on 2026-10-03.
# Copyright (c) 2026 Nina Alexandra Klama
#


# This file is part of DarkDrives.
#
# DarkDrives is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# DarkDrives is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with Foobar. If not, see
# <https://www.gnu.org/licenses/>.


cmake_minimum_required(VERSION 4.4)
project(DarkDrives)
include(FetchContent)

set(CMAKE_CXX_STANDARD 23)


find_package(Sqlpp23 REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(yaml-cpp REQUIRED)

add_executable(DarkDrives main.cpp
        fuse/fuse_interface.cpp
        fuse/fuse_interface.h
        config/config.cpp
        config/config.h
        config/drive_config.cpp
        config/drive_config.h
        logger.cpp
        logger.h
        database/db.cpp
        database/db.h
        database/database_worker.cpp
        database/database_worker.h
        config/exceptions.h)

target_link_libraries(DarkDrives PRIVATE sqlpp23::core)
target_link_libraries(DarkDrives PRIVATE sqlpp23::sqlite3)
#target_link_libraries(DarkDrives PRIVATE sqlite3)
target_link_libraries(DarkDrives PRIVATE yaml-cpp::yaml-cpp)
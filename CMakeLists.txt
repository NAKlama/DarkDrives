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
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(DarkDrives)
include(FetchContent)

set(CMAKE_CXX_STANDARD 26)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

#find_package(Sqlpp23 REQUIRED)
find_package(Sqlpp23 COMPONENTS MariaDB REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(MariaDBClient REQUIRED)
#find_library(FUSE_LIB fuse)
#find_package(FindPkgConfig REQUIRED)
#pkg_check_modules(mariadb REQUIRED IMPORTED_TARGET "mysqlclient")

set(CMAKE_CXX_SCAN_FOR_MODULES true)

#execute_process(COMMAND pwd WORKING_DIRECTORY .. COMMAND_ECHO STDOUT)

execute_process(COMMAND sqlpp23-ddl2cpp
        --path-to-ddl database/DarkDrives.mariadb.sql
        --namespace db_t
        --generate-table-creation-helper
        --naming-style identity
        --module-name db_tables
        --path-to-module database/db_tables.cppm
        WORKING_DIRECTORY ..
        INPUT_FILE database/DarkDrives.mariadb.sql
        OUTPUT_FILE database/db_tables.cppm
        COMMAND_ECHO STDOUT
)

add_executable(DarkDrives main.cpp
        fuse/fuse_interface.cpp
        fuse/fuse_interface.h
        config/config.cpp
        config/config.h
        config/drive_config.cpp
        config/drive_config.h
        database/db.cpp
        database/db.h
        database/database_worker.cpp
        database/database_worker.h
        config/exceptions.h
        database/db_config.h
        logger.h
        string_functions.cpp
        string_functions.h
        config/exceptions.cpp
        fuse/open_file.cpp
        fuse/open_file.h
        cache.h
        cache.h
        int_functions.h
)

set_target_properties(DarkDrives
        PROPERTIES
        CXX_SCAN_FOR_MODULES ON)

add_executable(test local/testing.cpp
        string_functions.cpp
        string_functions.h
)

target_compile_options(DarkDrives BEFORE PRIVATE -D_FILE_OFFSET_BITS=64)

target_link_libraries(DarkDrives PRIVATE fuse)
target_link_libraries(DarkDrives PRIVATE sqlpp23::core_module)
target_link_libraries(DarkDrives PRIVATE sqlpp23::core)
target_link_libraries(DarkDrives PRIVATE sqlpp23::mariadb_module)
target_link_libraries(DarkDrives PRIVATE sqlpp23::mariadb)
target_link_libraries(DarkDrives PRIVATE spdlog::spdlog)
#target_link_libraries(DarkDrives PRIVATE PkgConfig::mariadb)
target_link_libraries(DarkDrives PRIVATE yaml-cpp::yaml-cpp)

target_sources(DarkDrives PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
        database/db_tables.cppm
)

add_subdirectory(testing)
cmake_minimum_required(VERSION 4.4)
project(DarkDrives)

set(CMAKE_CXX_STANDARD 23)

add_executable(DarkDrives main.cpp
        fuse/fuse_interface.cpp
        fuse/fuse_interface.h
        config/config.cpp
        config/config.h
        config/drive_config.cpp
        config/drive_config.h
        logger.cpp
        logger.h)

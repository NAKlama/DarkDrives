cmake_minimum_required(VERSION 4.4)
project(DarkDrives)

set(CMAKE_CXX_STANDARD 23)


find_package(Sqlpp23 REQUIRED)
find_package(SQLite3 REQUIRED)


add_executable(DarkDrives main.cpp
        fuse/fuse_interface.cpp
        fuse/fuse_interface.h
        config/config.cpp
        config/config.h
        config/drive_config.cpp
        config/drive_config.h
        logger.cpp
        logger.h)

target_link_libraries(DarkDrives PRIVATE sqlpp23:sqlpp23)
target_link_libraries(DarkDrives PRIVATE sqlite3)
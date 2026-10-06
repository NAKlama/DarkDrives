//
// Created by Nina Alexandra Klama on 2026-10-05.
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
#ifndef DARKDRIVES_OPEN_FILES_H
#define DARKDRIVES_OPEN_FILES_H

// #include <cstdint>
#include <memory>
#include <string>

using namespace std;

class OpenFile
{
public:
    explicit OpenFile(unique_ptr<string> data);

    [[nodiscard]] string_view read(size_t offset, size_t size) const;
    [[nodiscard]] string_view read_all() const;
    [[nodiscard]] size_t size() const;

private:
    unique_ptr<string> data;
};


#endif //DARKDRIVES_OPEN_FILES_H

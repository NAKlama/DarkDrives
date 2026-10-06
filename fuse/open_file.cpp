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
#include "open_file.h"

#include <utility>

OpenFile::OpenFile(unique_ptr<string> data_in) : data(std::move(data_in)) {}

string_view OpenFile::read(size_t offset, size_t size) const
{
    return string_view(*(data)).substr(offset, size);
}

string_view OpenFile::read_all() const
{
    return string_view(*(data));
}

size_t OpenFile::size() const
{
    return data->size();
}

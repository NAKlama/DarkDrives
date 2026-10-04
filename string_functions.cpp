//
// Created by Nina Alexandra Klama on 2026-10-04.
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
#include "string_functions.h"



list<string> str_f::split_delimiter(char delimiter, const string& path)
{
    list<string> path_list;
    size_t offset = 0;
    size_t next_delim = 0;
    if (!path.empty() && path[0] == delimiter) ++offset;
    while ((next_delim = path.find(delimiter, offset)) != string::npos)
    {
        path_list.push_back(path.substr(offset, next_delim - offset));
        offset = next_delim + 1;
    }
    if (path.at(path.length() - 1) != delimiter)
        path_list.push_back(path.substr(offset, string::npos));
    return path_list;
}

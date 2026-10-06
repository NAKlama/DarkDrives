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

#include <sys/stat.h>
#include <sstream>

list<string> str_f::split_delimiter(const char delimiter, const string& path)
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

string str_f::join(const char delimiter, const list<string>& path_segments)
{
    if (path_segments.empty())
        return "";
    if (path_segments.size() == 1)
        return path_segments.front();
    stringstream ss;
    for (auto& path_segment : path_segments)
    {
        ss << path_segment << delimiter;
    }
    return ss.str().substr(0, ss.str().length() - 1);
}

string str_f::decode_mode_bits(uint16_t mode, const bool verbose)
{
    string out("----------");
    if (mode & S_ISVTX) out[0] = 't';
    if (mode & S_IRUSR) out[1] = 'r';
    if (mode & S_IWUSR) out[2] = 'w';
    if (mode & S_IXUSR)
    {
        if (mode & S_ISUID) out[3] = 's';
        else out[3] = 'x';
    }
    if (mode & S_IRGRP) out[4] = 'r';
    if (mode & S_IWGRP) out[5] = 'w';
    if (mode & S_IXGRP)
    {
        if (mode & S_ISGID) out[6] = 's';
        else out[6] = 'x';
    }
    if (mode & S_IROTH) out[7] = 'r';
    if (mode & S_IWOTH) out[8] = 'w';
    if (mode & S_IXOTH) out[9] = 'x';
    if (verbose)
    {
        stringstream ss;
        ss << " (" << std::oct << mode << ")";
        out.append(ss.str());
    }
    return out;
}

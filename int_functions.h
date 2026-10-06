//
// Created by Nina Alexandra Klama on 2026-10-06.
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
#ifndef DARKDRIVES_INT_FUNCTIONS_H
#define DARKDRIVES_INT_FUNCTIONS_H
#include <algorithm>
#include <cstdint>
#include <limits>


namespace int_f
{
    static int64_t cast_to_signed(const uint64_t value)
    {
        return static_cast<int64_t>(std::min(value, static_cast<uint64_t>(std::numeric_limits<int64_t>::max())));
    }

    static int32_t cast_to_signed(const uint32_t value)
    {
        return static_cast<int32_t>(std::min(value, static_cast<uint32_t>(std::numeric_limits<int32_t>::max())));
    }

    static int16_t cast_to_signed(const uint16_t value)
    {
        return static_cast<int16_t>(std::min(value, static_cast<uint16_t>(std::numeric_limits<int16_t>::max())));
    }

    static int8_t cast_to_signed(const uint8_t value)
    {
        return static_cast<int8_t>(std::min(value, static_cast<uint8_t>(std::numeric_limits<int8_t>::max())));
    }
}


#endif //DARKDRIVES_INT_FUNCTIONS_H

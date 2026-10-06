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
#ifndef DARKDRIVES_CACHE_H
#define DARKDRIVES_CACHE_H

#ifndef CACHE_MAP_DEFAULT_SIZE
#define CACHE_MAP_DEFAULT_SIZE 1024
#endif

#include <chrono>
#include <cstddef>
#include <memory>
#include <deque>
#include <functional>
#include <algorithm>

using namespace std;

template <typename T> struct cache_item
{
    chrono::time_point<chrono::system_clock> last_access;
    T data;
};

template <typename K, typename V> class Cache
{
private:
    std::size_t max_size_;
    deque<pair<K, cache_item<V>>> data_;
    function<V(K)> getter_;

public:
    Cache(function<V(K)> getter) : max_size_(CACHE_MAP_DEFAULT_SIZE), data_({}), getter_(getter) {}
    Cache(function<V(K)> getter, const size_t size) : max_size_(size), data_({}), getter_(getter) {}

    bool has_key(const K& key)
    {
        for (size_t i = 0; i < data_.size(); ++i)
        {
            if (data_[i].first == key)
                return true;
        }
        return false;
    }

    size_t size()
    {
        return data_.size();
    }

    V get(const K& key)
    {
        cache_item<V> value;
        for (size_t i = 0; i < data_.size(); ++i)
        {
            if (data_[i].first == key)
            {
                value = data_[i].second;
                data_[i].second.last_access = chrono::system_clock::now();
                return value.data;
            }
        }
        sort(data_.begin(), data_.end(),
            [](auto a, auto b)
            {
                return a.second.last_access < b.second.last_access;
            });
        while (data_.size() >= max_size_)
            data_.erase(data_.end());
        value = {
            .last_access = chrono::system_clock::now(),
            .data        = getter_(key)
        };
        data_.push_front({key, value});
        return value.data;
    }
};

#endif //DARKDRIVES_CACHE_H

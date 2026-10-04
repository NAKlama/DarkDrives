//
// Created by Nina Alexandra Klama on 2026-10-03.
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
#ifndef DARKDRIVES_EXCEPTIONS_H
#define DARKDRIVES_EXCEPTIONS_H
#include <exception>
#include <stdexcept>
#include <string>

using namespace std;

struct ExceptionNoConfigurationFound : std::exception
{
    [[nodiscard]] const char* what() const noexcept override { return "No configuration file found!"; };
};

struct ExceptionNoDatabaseConfigured : std::exception
{
    [[nodiscard]] const char* what() const noexcept override { return "No database configured!"; };
};

struct ExceptionConfigRootNotMap : std::exception
{
    [[nodiscard]] const char* what() const noexcept override { return "Config root is not a map!"; };
};

class ExceptionConfigDatatype : public std::runtime_error {
public:
    explicit ExceptionConfigDatatype(const string& string);
};

class ExceptionConfigMissingData : public std::runtime_error {
public:
    explicit ExceptionConfigMissingData(const string& string);
};





#endif //DARKDRIVES_EXCEPTIONS_H

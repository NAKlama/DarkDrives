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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "../string_functions.h"
#include <list>
#include <string>

using Catch::Matchers::RangeEquals;

TEST_CASE("split_delimiter splits cleanly at delimiter", "[split_delimiter]")
{
    list<string> expected = {"this", "is", "a", "test", "path"};

    SECTION("Using '/' as delimiter")
    {
        char delimiter = '/';
        SECTION("splitting without leading or trailing slashes")
        {
            string in_string = "this/is/a/test/path";

            REQUIRE_THAT( str_f::split_delimiter(delimiter, in_string), RangeEquals( expected ) );
        }

        SECTION("splitting with leading '/'")
        {
            string in_string = "/this/is/a/test/path";

            REQUIRE_THAT( str_f::split_delimiter(delimiter, in_string), RangeEquals( expected ) );
        }

        SECTION("splitting with trailing '/'")
        {
            string in_string = "this/is/a/test/path/";

            REQUIRE_THAT( str_f::split_delimiter(delimiter, in_string), RangeEquals( expected ) );
        }

        SECTION("splitting with both leading and trailing slashes")
        {
            string in_string = "/this/is/a/test/path/";

            REQUIRE_THAT( str_f::split_delimiter(delimiter, in_string), RangeEquals( expected ) );
        }
    }

    SECTION("Using ':' as delimiter")
    {
        char delimiter = ':';
        SECTION("splitting without leading or trailing colons")
        {
            string in_string = "this:is:a:test:path";

            REQUIRE_THAT( str_f::split_delimiter(delimiter, in_string), RangeEquals( expected ) );
        }

        SECTION("splitting with leading ':'")
        {
            string in_string = ":this:is:a:test:path";

            REQUIRE_THAT( str_f::split_delimiter(delimiter, in_string), RangeEquals( expected ) );
        }

        SECTION("splitting with trailing ':'")
        {
            string in_string = "this:is:a:test:path:";

            REQUIRE_THAT( str_f::split_delimiter(delimiter, in_string), RangeEquals( expected ) );
        }

        SECTION("splitting with both leading and trailing colons")
        {
            string in_string = ":this:is:a:test:path:";

            REQUIRE_THAT( str_f::split_delimiter(delimiter, in_string), RangeEquals( expected ) );
        }
    }
}
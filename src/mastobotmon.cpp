/*  This file is part of mastobotmon.
 *  Copyright © 2018 tastytea <tastytea@tastytea.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string>
#include <cstdint>
#include <mastodon-cpp.hpp>
#include <rapidjson/document.h>
#include "version.hpp"
#include "mastobotmon.hpp"

using Mastodon::API;
using std::cout;
using std::cerr;
using std::cin;
using std::string;

int main(int argc, char *argv[])
{
    rapidjson::Document document;
    if (!read_config(document))
    {
        return 1;
    }

    for (const auto &member : document["accounts"].GetObject())
    {
        cout << member.name.GetString() << ": " << member.value.GetUint() << '\n';
    }

    return 0;
}

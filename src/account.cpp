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
#include <mastodon-cpp/mastodon-cpp.hpp>
#include "version.hpp"
#include "mastobotmon.hpp"

using std::cout;
using std::cerr;
using std::cin;
using std::string;
using std::uint16_t;

Account::Account(const string &instance, const string &access_token)
: API(instance, access_token)
, _minutes(0)
, _last_mention_id(0)
{
    //
}

const void Account::set_minutes(uint16_t minutes)
{
    _minutes = minutes;
}

const uint16_t Account::get_minutes() const
{
    return _minutes;
}

const uint16_t Account::get_mentions(string &answer)
{
    parametermap parameters =
    {
        { "since_id", { std::to_string(_last_mention_id) } },
        { "exclude_types", { "follow", "favourite", "reblog" } }
    };
    return get(v1::notifications, parameters, answer);
}

const void Account::set_last_mention_id(const std::uint64_t &id)
{
    _last_mention_id = id;
}

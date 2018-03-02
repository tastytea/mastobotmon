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

#ifndef mastobotmon_HPP
#define mastobotmon_HPP

#include <string>
#include <cstdint>
#include <rapidjson/document.h>
#include <mastodon-cpp.hpp>

using std::uint16_t;
using std::string;

bool read_config(rapidjson::Document &document);
const string get_access_token(const string &account);

class Account : public Mastodon::API
{
public:
    explicit Account(const string &instance, const string &access_token);
    const void set_minutes(uint16_t minutes);
    const uint16_t get_minutes() const;

private:
    uint16_t _minutes;
};

#endif // mastobotmon_HPP

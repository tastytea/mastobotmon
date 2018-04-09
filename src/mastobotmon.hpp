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
#include <jsoncpp/json/json.h>
#include <mastodon-cpp/mastodon-cpp.hpp>

using std::uint16_t;
using std::string;

extern Json::Value config;

class Account : public Mastodon::API
{
public:
    explicit Account(const string &instance, const string &access_token);
    const void set_minutes(uint16_t minutes);
    const uint16_t get_minutes() const;
    const uint16_t get_mentions(string &answer);
    const void set_last_mention_id(const std::uint64_t &id);

private:
    uint16_t _minutes;
    std::uint64_t _last_mention_id;
};

const bool read_config();
const string get_access_token(const string &account);
const bool add_account();
const bool write_config();

const bool write_mentions(const string &straccount, Json::Value &mentions);
const bool write_statistics(const string &straccount, Json::Value &account_json);

#endif // mastobotmon_HPP

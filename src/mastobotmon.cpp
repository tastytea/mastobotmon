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
#include <vector>
#include <rapidjson/document.h>
#include "version.hpp"
#include "mastobotmon.hpp"

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

    std::vector<Account> accounts;

    for (const auto &member : document["accounts"].GetObject())
    {
        // Construct an Account object for every account and store it in a vector
        string instance = member.name.GetString();
        instance = instance.substr(instance.find('@') + 1);

        Account *acc = new Account(instance, member.value["access_token"].GetString());
        acc->set_useragent("mastobotmon/" + string(global::version));
        acc->set_minutes(member.value["minutes"].GetUint());
        accounts.push_back(*acc);
    }

    if (document["mode"] == "cron")
    {
        for (Account &acc : accounts)
        {
            std::string answer;
            uint16_t ret = acc.get(Mastodon::API::v1::accounts_verify_credentials, answer);
            if (ret == 0)
            {
                rapidjson::Document json;
                json.Parse(answer.c_str());
                const string id = json["id"].GetString();

                Account::parametermap parameters(
                {
                    { "limit", { "1" } }
                });
                ret = acc.get(Mastodon::API::v1::accounts_id_statuses, id, parameters, answer);
                if (ret == 0)
                {
                    json.Parse(answer.c_str());
                    cout << "The last toot of " << json[0]["account"]["acct"].GetString()
                         << " was at " << json[0]["created_at"].GetString() << ".\n";
                }
            }

            if (ret != 0)
            {
                cerr << "Error: " << ret << '\n';
            }
        }
    }

    return 0;
}

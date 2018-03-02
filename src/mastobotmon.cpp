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

#define RAPIDJSON_HAS_STDSTRING 1

#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>  // get_time
#include <rapidjson/document.h>
#include "version.hpp"
#include "mastobotmon.hpp"

using std::cout;
using std::cerr;
using std::cin;
using std::string;
using std::uint16_t;

int main(int argc, char *argv[])
{
    rapidjson::Document document;
    if (!read_config(document))
    {
        return 1;
    }

    if (argc > 1)
    {
        if ((std::strncmp(argv[1], "add", 3)) == 0)
        {
            add_account(document);
        }
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
                    const string acct = json[0]["account"]["acct"].GetString();

                    std::istringstream isslast(json[0]["created_at"].GetString());
                    struct std::tm tm = {0};
                    isslast >> std::get_time(&tm, "%Y-%m-%dT%T");
                    std::time_t time = mktime(&tm);

                    const auto now = std::chrono::system_clock::now();
                    const auto last = std::chrono::system_clock::from_time_t(time);
                    auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - last);

                    if (elapsed.count() > acc.get_minutes())
                    {
                        cout << "ALERT: " << acct << " is inactive since " << elapsed.count() << " minutes.\n";
                    }
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

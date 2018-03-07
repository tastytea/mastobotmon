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
#include <cstring>
#include <cstdint>
#include <vector>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>  // get_time
#include <jsoncpp/json/json.h>
#include "version.hpp"
#include "mastobotmon.hpp"

using std::cout;
using std::cerr;
using std::cin;
using std::string;
using std::uint16_t;

int main(int argc, char *argv[])
{
    Json::Value document;
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

    for (auto it = document["accounts"].begin(); it != document["accounts"].end(); ++it)
    {
        // Construct an Account object for every account and store it in a vector
        string instance = it.name();
        instance = instance.substr(instance.find('@') + 1);

        Account *acc = new Account(instance, (*it)["access_token"].asString());
        acc->set_useragent("mastobotmon/" + string(global::version));
        acc->set_minutes((*it)["minutes"].asUInt());
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
                Json::Value json;
                Json::Reader reader;
                reader.parse(answer, json);
                const string id = json["id"].asString();

                Account::parametermap parameters(
                {
                    { "limit", { "1" } }
                });
                ret = acc.get(Mastodon::API::v1::accounts_id_statuses, id, parameters, answer);
                if (ret == 0)
                {
                    reader.parse(answer, json);
                    const string acct = json[0]["account"]["acct"].asString();

                    std::istringstream isslast(json[0]["created_at"].asString());
                    struct std::tm tm = {0};
                    isslast >> std::get_time(&tm, "%Y-%m-%dT%T");
                    std::time_t time = timegm(&tm);

                    const auto now = std::chrono::system_clock::now();
                    const auto last = std::chrono::system_clock::from_time_t(time);
                    auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - last);

                    if (elapsed.count() > acc.get_minutes())
                    {
                        uint16_t minutes = elapsed.count();
                        std::uint8_t hours = 0;
                        std:: uint8_t days = 0;
                        while (minutes >= 1440)
                        {
                            minutes -= 1440;
                            days += 1;
                        }
                        while (minutes >= 60)
                        {
                            minutes -= 60;
                            hours += 1;
                        }
                        cout << "ALERT: " << acct << " is inactive since ";
                        if (days > 0)
                        {
                            cout << std::to_string(days) << " days, ";
                        }
                        if (hours > 0)
                        {
                            cout << std::to_string(hours) << " hours and ";
                        }
                        cout << std::to_string(minutes) << " minutes.\n";
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

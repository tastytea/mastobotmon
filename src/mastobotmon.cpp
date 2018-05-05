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
#include <fstream>
#include <regex>
#include <memory>
#include <jsoncpp/json/json.h>
#include <mastodon-cpp/easy/entities/account.hpp>
#include <mastodon-cpp/easy/entities/notification.hpp>
#include "version.hpp"
#include "mastobotmon.hpp"

using std::cout;
using std::cerr;
using std::cin;
using std::string;
using std::uint16_t;
using Mastodon::Easy;
using std::chrono::system_clock;

Json::Value config; // Declared in mastobotmon.hpp

const bool write_mentions(const string &straccount, std::vector<std::shared_ptr<Easy::Notification>> &mentions)
{
    const string filepath = config["data_dir"].asString() + "/mentions_" + straccount + ".csv";
    const std::regex restrip("<[^>]*>");

    std::ofstream outfile(filepath, std::ios::app);
    if (outfile.is_open())
    {
        string output;
        for (std::shared_ptr<Easy::Notification> &mention : mentions)
        {
            output = mention->status().account().acct() + ';';
            output += Easy::strtime_utc(mention->status().created_at(), "%T") + ';';
            output += mention->status().content() + ';';
            output += mention->status().url() + '\n';
            output = std::regex_replace(output, restrip, "");
            outfile.write(output.c_str(), output.length());
        }
        outfile.close();
        cout << "New mentions in: " << filepath << '\n';

        return true;
    }

    cerr << "Error writing file: " << filepath << '\n';
    return false;
}

const bool write_statistics(const string &straccount, Easy::Account &account_entity)
{
    const string filepath = config["data_dir"].asString() + "/statistics_" + straccount + ".csv";

    std::ofstream outfile(filepath, std::ios::app);
    if (outfile.is_open())
    {
        string output;
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::time_t now_t = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_t);
        std::stringstream ss;

        ss << std::put_time(&now_tm, "%Y-%m-%dT%T");
        output = ss.str() + ';';
        output += std::to_string(account_entity.statuses_count()) + ';';
        output += std::to_string(account_entity.followers_count()) + '\n';
        outfile.write(output.c_str(), output.length());
        outfile.close();

        return true;
    }

    cerr << "Error writing file: " << filepath << '\n';
    return false;
}

int main(int argc, char *argv[])
{
    uint16_t mainret = 0;
    if (!read_config())
    {
        return 1;
    }

    if (argc > 1)
    {
        if ((std::strncmp(argv[1], "add", 3)) == 0)
        {
            add_account();
        }
    }

    std::vector<std::shared_ptr<Account>> accounts;

    for (auto it = config["accounts"].begin(); it != config["accounts"].end(); ++it)
    {
        // Construct an Account object for every account and store it in a vector
        string instance = it.name();
        instance = instance.substr(instance.find('@') + 1);

        std::shared_ptr<Account> acc(std::make_shared<Account>(instance, (*it)["access_token"].asString()));
        //Account *acc = new Account(instance, (*it)["access_token"].asString());
        acc->set_useragent("mastobotmon/" + string(global::version));
        acc->set_minutes((*it)["minutes"].asUInt());
        if (!(*it)["last_mention"].empty())
        {
            acc->set_last_mention_id((*it)["last_mention"].asUInt64());
        }
        accounts.push_back(acc);
    }

    if (config["mode"] == "cron")
    {
        for (std::shared_ptr<Account> &acc : accounts)
        {
            std::string answer;
            uint16_t ret = acc->get(Mastodon::API::v1::accounts_verify_credentials, answer);
            if (ret == 0)
            {
                if (!acc->get_header("X-RateLimit-Remaining").empty() &&
                    std::stoi(acc->get_header("X-RateLimit-Remaining")) < 2)
                {
                    cerr << "ERROR: Reached limit of API calls.\n";
                    cerr << "Counter will reset at " << acc->get_header("X-RateLimit-Reset")
                         << '\n';
                    return 2;
                }
                Easy::Account account_entity(answer);
                const string id = std::to_string(account_entity.id());
                const string straccount = account_entity.acct() + "@" + acc->get_instance();
                write_statistics(straccount, account_entity);

                Account::parametermap parameters(
                {
                    { "id", { id } },
                    { "limit", { "1" } }
                });
                ret = acc->get(Mastodon::API::v1::accounts_id_statuses, parameters, answer);
                if (ret == 0)
                {
                    const Easy::Status status(answer);
                    const auto now = std::chrono::system_clock::now();
                    const auto last = status.created_at();
                    auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - last);

                    if (elapsed.count() > acc->get_minutes())
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
                        cout << "ALERT: " << account_entity.acct() << " is inactive since ";
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

                    ret = acc->get_mentions(answer);
                    if (ret == 0)
                    {
                        std::vector<std::shared_ptr<Easy::Notification>> notifications;
                        for (const string &str : Easy::json_array_to_vector(answer))
                        {
                            notifications.push_back(std::make_shared<Easy::Notification>(str));
                        }
                        if (!notifications.empty())
                        {
                            const std::uint64_t lastid = notifications[0]->id();
                            acc->set_last_mention_id(lastid);
                            config["accounts"][straccount]["last_mention"] = lastid;
                            write_mentions(straccount, notifications);
                        }
                    }
                }
            }

            if (ret != 0)
            {
                cerr << "Error: " << ret << '\n';
                mainret = ret;
            }
        }
    }

    if (!write_config())
    {
        cerr << "Couldn't write config file\n";
    }
    return mainret;
}

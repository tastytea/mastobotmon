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
#include <cstdlib>  // getenv()
#include <fstream>
#include <sstream>
#include <jsoncpp/json/json.h>
#include "version.hpp"
#include "mastobotmon.hpp"

using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::uint16_t;

const string filepath = string(std::getenv("HOME")) + "/.config/mastobotmon.json";

const bool read_config()
{
    std::ifstream file(filepath);
    std::stringstream json;

    if (file.is_open())
    {
        json << file.rdbuf();
        file.close();

        Json::Reader reader;

        if (!reader.parse(json, config))
        {
            cerr << "ERROR: couldn't parse config file. Are you sure the JSON is well-formed?\n";
            return false;
        }

        if (!config["accounts"].isObject())
        {
            cerr << "ERROR: \"accounts\" not found\n";
            return false;
        }

        if (!config["mode"].isString())
        {
            cerr << "ERROR: \"mode\" not found\n";
            return false;
        }

        if (!config["daemon_check"].isUInt())
        {
            cerr << "ERROR: \"daemon_check\" not found\n";
            return false;
        }

        if (!config["data_dir"].isString())
        {
            cerr << "ERROR: \"data_dir\" not found\n";
            return false;
        }
    }
    else
    {
        cout << "No config file found. Creating new one.\n";

        add_account();

        config["mode"] = "cron";
        config["daemon_check"] = 60;
        config["data_dir"] = ".";

        return write_config();
    }

    return true;
}

const string get_access_token(const string &account)
{
    const string instance = account.substr(account.find('@') + 1);
    Account acc(instance, "");
    acc.set_useragent("mastobotmon/" + string(global::version));
    uint16_t ret;
    string client_id;
    string client_secret;
    string url;
    string code;
    string access_token = "";

    ret = acc.register_app1("mastobotmon", "urn:ietf:wg:oauth:2.0:oob", "read",
                            "https://github.com/tastytea/mastobotmon",
                            client_id, client_secret, url);
    if (ret == 0)
    {
        cout << "Visit " << url << " and paste the generated code\nhere: ";
        cin >> code;
        ret = acc.register_app2(client_id, client_secret, "urn:ietf:wg:oauth:2.0:oob",
                                code, access_token);
        if (ret == 0)
        {
            return access_token;
        }
    }

    cerr << "Error: " << ret << '\n';
    return "";
}

const bool add_account()
{
    string account;
    string minutes;
    string access_token;

    cout << "Adding accounts (user@domain), blank line to stop.\n";
    while (true)
    {
        cout << "Add Account: ";
        std::getline(cin, account);
        if (account.empty())
        {
            break;
        }
        cout << "Minutes af allowed inactivity: ";
        std::getline(cin, minutes);
        access_token = get_access_token(account);

        config["accounts"][account]["minutes"] = std::stoi(minutes);
        config["accounts"][account]["access_token"] = access_token;
    }

    return write_config();
}

const bool write_config()
{
    Json::StyledWriter writer;
    const string output = writer.write(config);

    std::ofstream outfile(filepath);
    if (outfile.is_open())
    {
        outfile.write(output.c_str(), output.length());
        outfile.close();

        return true;
    }

    return false;
}

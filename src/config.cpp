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
#include <cstring>  // strlen()
#include <cstdlib>  // getenv()
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include "version.hpp"
#include "mastobotmon.hpp"

using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::uint16_t;

bool read_config(rapidjson::Document &document)
{
    const string filepath = string(std::getenv("HOME")) + "/.config/mastobotmon.json";
    std::ifstream file(filepath);
    std::stringstream json;

    if (file.is_open())
    {
        json << file.rdbuf();
        file.close();

        if (document.Parse(json.str().c_str()).HasParseError())
        {
            return false;
        }

        if (!document["accounts"].IsObject())
        {
            cerr << "ERROR: \"accounts\" not found\n";
            return false;
        }

        if (!document["mode"].IsString())
        {
            cerr << "ERROR: \"mode\" not found\n";
            return false;
        }

        if (!document["daemon_check"].IsUint())
        {
            cerr << "ERROR: \"daemon_check\" not found\n";
            return false;
        }
    }
    else
    {
        cout << "No config file found. Creating new one.\n";

        std::ofstream outfile(filepath);
        if (outfile.is_open())
        {
            cout << "Adding accounts (user@domain), blank line to stop.\n";
            rapidjson::StringBuffer buffer;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            string account;
            string minutes;

            writer.StartObject();
            writer.Key("accounts");
            writer.StartObject();

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
                writer.Key(account.c_str());
                writer.StartObject();
                writer.Key("minutes");
                writer.Uint(std::stoi(minutes));
                writer.Key("access_token");
                writer.String(get_access_token(account).c_str());
                writer.EndObject();
            }
            writer.EndObject();

            writer.Key("mode");
            writer.String("cron");
            writer.Key("daemon_check");
            writer.Uint(10);
            writer.EndObject();

            outfile.write(buffer.GetString(), std::strlen(buffer.GetString()));
            outfile.close();

            if (!document.Parse(buffer.GetString()).HasParseError())
            {
                return true;
            }
        }

        return false;
    }

    return true;
}

const string get_access_token(const string &account)
{
    const string instance = account.substr(account.find('@') + 1);
    Account acc(instance, "");
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

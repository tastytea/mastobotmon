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

const string filepath = string(std::getenv("HOME")) + "/.config/mastobotmon.json";

const bool read_config(rapidjson::Document &document)
{
    std::ifstream file(filepath);
    std::stringstream json;

    if (file.is_open())
    {
        json << file.rdbuf();
        file.close();

        if (document.Parse(json.str().c_str()).HasParseError())
        {
            cerr << "ERROR: couldn't parse config file. Are you sure the JSON is well-formed?\n";
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

        rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

        document.SetObject();
        rapidjson::Value object(rapidjson::kObjectType);
        document.AddMember("accounts", object.Move(), allocator);
        add_account(document);

        document.AddMember("mode", "cron", allocator);
        document.AddMember("daemon_check", 10, allocator);

        return write_config(document);
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

const bool add_account(rapidjson::Document &document)
{
    string account;
    string minutes;
    string access_token;
    rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

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

        rapidjson::Value vobject(rapidjson::kObjectType);
        rapidjson::Value vaccount(account, allocator);
        rapidjson::Value vaccess_token(access_token, allocator);

        vobject.AddMember("minutes", std::stoi(minutes), allocator);
        vobject.AddMember("access_token", vaccess_token.Move(), allocator);
        document["accounts"].AddMember(vaccount.Move(), vobject, allocator);
    }

    return write_config(document);
}

const bool write_config(rapidjson::Document &document)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::ofstream outfile(filepath);
    if (outfile.is_open())
    {
        outfile.write(buffer.GetString(), std::strlen(buffer.GetString()));
        outfile.close();

        return true;
    }

    return false;
}

**mastobotmon** monitors your bots on Mastodon.

# Install

## Dependencies

 * Tested OS: Linux
 * C++ compiler (tested: gcc 6.4, clang 5.0)
 * [cmake](https://cmake.org/) (tested: 3.9.6)
 * [mastodon-cpp](https://github.com/tastytea/mastodon-cpp) (at least: 0.10.1)
 * [jsoncpp](https://github.com/open-source-parsers/jsoncpp) (tested: 1.8.1)

## Get sourcecode

### Development version

    git clone https://github.com/tastytea/mastobotmon.git

## Compile

    mkdir build
    cd build/
    cmake ..
    make

cmake options:

 * `-DCMAKE_BUILD_TYPE=Debug` for a debug build

Install with `make install`.

# Usage

Run mastobotmon once. Edit config file (`~/.config/mastobotmon.json`).
Call mastobotmon from cron. If a bot is inactive for longer than the specified
minutes, it prints an alert to stdout.

## Commands
* add – Add accounts

## Error codes

Same as [mastodon-cpp](https://github.com/tastytea/mastodon-cpp/blob/master/README.md#error-codes), plus:

|      Code | Explanation                   |
| --------: |:------------------------------|
|         1 | Couldn't read / create config |
|         2 | Reached limit of API calls    |

If you use a debug build, you get more verbose error messages.

## Example config file

    {
       "accounts" : {
          "account1@example.social" : {
             "access_token" : "xxxx",
             // Alert after this many minutes of inactivity
             "minutes" : 720
          },
          "account2@example.social" : {
             "access_token" : "yyyy",
             "minutes" : 1450
          }
       },
       // In daemon mode, check every x minutes
       "daemon_check" : 60,
       // This is the directory where the mentions and statistics will be stored.
       "data_dir" : "/home/user/mastobotmon",
       // Possible values: cron (daemon mode later)
       "mode" : "cron"
    }

## Mentions

Mentions are written to `data_dir/mentions_account.csv`. The format is: acct;created_at;content;url.

## Statistics

Statistics are written to `data_dir/statistics_account.csv`. The format is: time;toots;followers.

# TODO

* Version 0.1.0
    * [x] Cron mode
    * [x] Config file
    * [x] Alert if account seems inactive
* Version 0.2.0
    * [x] Allow to add accounts later
    * [x] Write mentions to file
* Version 0.3.0
    * [x] Respect X-RateLimit header
    * [x] Write statistics to file
* Version 0.4.0
    * [ ] Daemon mode
    * [ ] Options to turn mention grabbing and statistics off
* Version 0.5.0
    * [ ] Allow to run command on alert

# Copyright

    Copyright © 2018 tastytea <tastytea@tastytea.de>.
    License GPLv3: GNU GPL version 3 <https://www.gnu.org/licenses/gpl-3.0.html>.
    This program comes with ABSOLUTELY NO WARRANTY. This is free software,
    and you are welcome to redistribute it under certain conditions.

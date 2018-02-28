**mastobotmon** monitors your bots on Mastodon.

# Install

## Dependencies

 * Tested OS: Linux
 * C++ compiler (tested: gcc 6.4, clang 5.0)
 * [cmake](https://cmake.org/) (tested: 3.9.6)
 * [mastodon-cpp](https://github.com/tastytea/mastodon-cpp) (tested: 0.4.4)
 * [rapidjson](http://rapidjson.org/) (tested: 1.1.0)

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
Call mastobotmon from cron.

# TODO

* Version 0.1.0
    * [ ] Cron mode
    * [x] Config file
    * [ ] Alert if account seems inactive
* Version 0.2.0
    * [ ] Write mentions to file
* Version 0.3.0
    * [ ] Write statistics to file
* Version 0.4.0
    * [ ] Daemon mode
* Version 0.5.0
    * [ ] Allow to run command on alert

## Error codes

Same as [mastodon-cpp](https://github.com/tastytea/mastodon-cpp/blob/master/README.md#error-codes)

If you use a debug build, you get more verbose error messages.

# Copyright

    Copyright © 2018 tastytea <tastytea@tastytea.de>.
    License GPLv3: GNU GPL version 3 <https://www.gnu.org/licenses/gpl-3.0.html>.
    This program comes with ABSOLUTELY NO WARRANTY. This is free software,
    and you are welcome to redistribute it under certain conditions.

# Bazcal Light Falcon

A native Bazcal Discord client using [libbazcal](https://github.com/Wykerd/libbazcal), written in C++.

# Build from source

Bazcal LF currently only supports GNU/Linux. Feel free to contribute if you wish for us to support other platforms.

Building from source requires [CMake](https://cmake.org) to generate the Makefiles.

## Dependencies to install

- [OpenSSL](https://www.openssl.org)
- [zlib](https://zlib.net)

All other dependencies are built from source as git submodules.

## Configure

Config is stored in a C++ header.

You need to add your bot token in this config file located at `include/config.hpp` in the define called `BAZCAL_BOT_TOKEN`

## Compiling

After installing the dependencies use CMake and build the project

```
git clone --recursive https://github.com/Wykerd/bazcal-lf.git
mkdir build
cd build
cmake ..
make
```

## Running

A binary called `bazcal` is generated. Execute it to start the bot.

# License

Copyright Daniel Wykerd 2020

```
Bazcal is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Bazcal is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with Bazcal.  If not, see <https://www.gnu.org/licenses/>.
```

**NOTE:** This software depends on other packages that may be licensed under different open source licenses.
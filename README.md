Pion Network Library
====================

Embedded HTTP server based on a source code from https://github.com/splunk/pion


Retrieving the code
-------------------

    git clone https://github.com/dnybz/pion.git
    cd pion


Building the code
-----------------

*For XCode:* use `pion.xcodeproj`

*For Visual Studio 2017 or later:* use `pion.sln`
1. Download openssl http://slproweb.com/download/Win32OpenSSL_Light-1_1_1k.exe
2. Install Win32OpenSSL_Light-1_1_1k.exe to C:\OpenSSL-Win32
3. Use the vs command line "Developer Command Prompt for VS 2017" to compile pion\third_party\zlib\build.bat
4. Use the vs2017 open `pion.sln` to do just it

On Unix platforms (including Linux, OSX, etc.) you can run

    ./autogen.sh
    ./configure

to generate Makefiles using GNU autotools, followed by

    make

to build everything except the unit tests.

You can build and run all the unit tests with

    make check

Generate build using CMake
---------------------------
[CMake](http://www.cmake.org) is cross-platform build generator.
Pion required cmake version 2.8.10+

To generate build call 

    cmake <path to pion clone> [-G <generator name>] [-D<option>...]
    
`-DZLIB_ROOT=<path to installed zlib>
-DOPENSSL_ROOT_DIR=...
-DLOG4CPLUS_ROOT=...`

Third Party Libraries
---------------------
For logging, Pion may be configured to:
* use std::cout and std::cerr for logging (the default configuration)

* use one of the following open source libraries:
    * [log4cplus](http://log4cplus.sourceforge.net/) (recommended; `--with-log4cplus`),
    * [log4cxx](http://logging.apache.org/log4cxx/) (`--with-log4cxx`) or
	* [log4cpp](http://log4cpp.sourceforge.net/) (`--with-log4cpp`).

* disable logging entirely (run `configure --disable-logging`)

Detailed build instructions are available for all of the platforms
supported by Pion within the `doc` subdirectory (`README.platform`).


License
-------
Copyright &copy; 2021 Wang Qiang
Copyright &copy; 2007-2016 Splunk Inc.

The Pion Network Library is published under the
[Boost Software License](http://www.boost.org/users/license.html).
See COPYING for licensing information.

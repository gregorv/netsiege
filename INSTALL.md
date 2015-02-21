
COMPILING AND INSTALLING NETSIEGE
===================================

1) DEPENDENCIES
---------------

You need the following tools to compile Netsiege:

* CMake (>= 2.8.8)
* g++ (>= 4.9), clang++ (>= 3.5) or some C++ compiler with decent C++11 support
* Protobuf Compiler (should be included in most library releases, see below)

You need the following libraries to compile Netsiege:

* Boost[system,filesystem] (definitely works with >=1.57, perhaps older
  versions work, too)
* Ogre (=1.9)
* OIS (maybe ;-))
* Google Protobuf (>= 2.6)

If you want to build unit tests or the Netsiege Editor, you also need

* Qt5 :D


2) COMPILING
------------

### LINUX

Compiling is done via the usual CMake process, so first go into the source
directory and

		mkdir build
		cd build
		cmake ..

If something goes wrong here, you either does not have one of the above
dependencies installed, the above list is incomplete, or CMake just can't find
the library files on your system.

In case you built OGRE from source and  did not install it globaly, you need
the set the OGRE_HOME variable, pointing to your OGRE installation.

		cmake .. -DOGRE_HOME=/path/to/ogre

To compile the Editor, you have to tell CMake by

		cmake .. -DBUILD_EDITOR=TRUE

When CMake found everything, you just need to run
		make

E voila, you got yourself some delicious Netsiege :-)

[Installing on the system should work, but was never tested. GFX, campaigns
etc are not yet included. Long story short, WIP]

### WINDOWS

HAHA, you're in trouble! Either tell me how to compile stuff on Windows
without rectal pain, or come back later when I know how.

3) BINARIES
-----------

Maybe later...
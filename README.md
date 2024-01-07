# MMORPG Server Emulator

Check the LICENSE first!

## External Dependencies

A copy of the used library versions can be found inside /Libraries folder

- MariaDB
- OpenSSL
- Zlib

## Database Setup

See /SQL folder for AuthDB and MasterDB setup and create two separate databases for each.

## Config Setup

Copy the contents of /Config folder to the location of the server executables and alter the content to match your setup.

## Client Setup

For any issues setting up client search on forums

1. Load the latest client version (tested with NA) and install it
2. Configure the Client /Data folder path inside your WorldSvr.ini for RuntimeDataPath
3. Configure the Server /ServerData folder path inside your WorldSvr.ini for ServerDataPath
4. Create an internal.txt file inside the /Data folder of your client and configure its content to connect to your server ip
5. Start the client executable from the Bin/x86 folder with the breaklee command

## Build

The project uses CMake as build system and only Win32 was tested for now.
For Linux distributions some smaller refactorings are required.

### Windows 64 Bit

```sh
cmake -G "Visual Studio 17 2022" -A x64 -S . -B "Build"
```

## State

The project is far away from begin finished and only implements a few things for now.
The WorldSvr is taking up 3gb of RAM because who cares at this point... if your system has low memory then refactor the ServerContext and ServerDataLoader.

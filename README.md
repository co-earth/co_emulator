# Cabal Online Server Emulator

Disclaimer: This project is an MMORPG server emulator created solely for educational purposes. It does not use any copyrighted materials or proprietary game-related information. The creator takes no responsibility for any misuse of this project for activities that may infringe on copyrights, proprietary rights, or violate terms of service agreements. 

This project is released under the MIT License.

## Project Motivation

This project was initiated out of a genuine interest on learning how game servers work and how they are or could be developed. 
Please note that this project is not affiliated with the official game developers, and any similarities if there are any are coincidental. The creator takes no responsibility for the use of this project for any purpose other than educational. The project is released under the MIT License, and users are encouraged to adhere to its terms.

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

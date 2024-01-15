cmake_minimum_required(VERSION 3.21.0)

project(Earth VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG=1")

if(WIN32)
	if(CMAKE_CL_64)
   		set(PLATFORM_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/Libraries/Win64)
   	else()
   		set(PLATFORM_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/Libraries/Win32)
	endif()
else()
	message(FATAL_ERROR "Platform not supported!")
endif()

set(MARIADB_INCLUDE_DIR ${PLATFORM_LIBRARY_DIR}/MariaDB/include)
set(OPENSSL_INCLUDE_DIR ${PLATFORM_LIBRARY_DIR}/OpenSSL/include)
set(ZLIB_INCLUDE_DIR ${PLATFORM_LIBRARY_DIR}/Zlib/include)

link_directories(
	${PLATFORM_LIBRARY_DIR}/MariaDB/lib
	${PLATFORM_LIBRARY_DIR}/OpenSSL/lib
	${PLATFORM_LIBRARY_DIR}/Zlib/lib
)

file(GLOB RUNTIME_LIBRARIES ${PLATFORM_LIBRARY_DIR}/MariaDB/lib/*.dll
		  					${PLATFORM_LIBRARY_DIR}/OpenSSL/lib/*.dll
		  					${PLATFORM_LIBRARY_DIR}/Zlib/lib/*.dll)

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/)

file(COPY ${RUNTIME_LIBRARIES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/)

file(COPY ${PROJECT_SOURCE_DIR}/ServerData/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/Data/)

set(AUCTION_SVR_DIR ${PROJECT_SOURCE_DIR}/AuctionSvr)
set(AUTH_SVR_DIR ${PROJECT_SOURCE_DIR}/AuthSvr)
set(CHAT_SVR_DIR ${PROJECT_SOURCE_DIR}/ChatSvr)
set(CORE_LIB_DIR ${PROJECT_SOURCE_DIR}/CoreLib)
set(MASTER_SVR_DIR ${PROJECT_SOURCE_DIR}/MasterSvr)
set(NET_LIB_DIR ${PROJECT_SOURCE_DIR}/NetLib)
set(RUNTIME_DATA_LIB_DIR ${PROJECT_SOURCE_DIR}/RuntimeDataLib)
set(RUNTIME_LIB_DIR ${PROJECT_SOURCE_DIR}/RuntimeLib)
set(WORLD_SVR_DIR ${PROJECT_SOURCE_DIR}/WorldSvr)

file(GLOB AUCTION_SVR_HEADERS ${AUCTION_SVR_DIR}/*.h)
file(GLOB AUCTION_SVR_SOURCES ${AUCTION_SVR_DIR}/*.c)

file(GLOB AUTH_SVR_HEADERS ${AUTH_SVR_DIR}/*.h)
file(GLOB AUTH_SVR_SOURCES ${AUTH_SVR_DIR}/*.c)

file(GLOB CHAT_SVR_HEADERS ${CHAT_SVR_DIR}/*.h)
file(GLOB CHAT_SVR_SOURCES ${CHAT_SVR_DIR}/*.c)

file(GLOB CORE_LIB_HEADERS ${CORE_LIB_DIR}/*.h)
file(GLOB CORE_LIB_SOURCES ${CORE_LIB_DIR}/*.c)

file(GLOB MASTER_SVR_HEADERS ${MASTER_SVR_DIR}/*.h)
file(GLOB MASTER_SVR_SOURCES ${MASTER_SVR_DIR}/*.c)

file(GLOB NET_LIB_HEADERS ${NET_LIB_DIR}/*.h)
file(GLOB NET_LIB_SOURCES ${NET_LIB_DIR}/*.c)

file(GLOB RUNTIME_DATA_LIB_HEADERS ${RUNTIME_DATA_LIB_DIR}/*.h)
file(GLOB RUNTIME_DATA_LIB_SOURCES ${RUNTIME_DATA_LIB_DIR}/*.c)

file(GLOB RUNTIME_LIB_HEADERS ${RUNTIME_LIB_DIR}/*.h)
file(GLOB RUNTIME_LIB_SOURCES ${RUNTIME_LIB_DIR}/*.c)

file(GLOB WORLD_SVR_HEADERS ${WORLD_SVR_DIR}/*.h)
file(GLOB WORLD_SVR_SOURCES ${WORLD_SVR_DIR}/*.c)

set(SHARED_HEADERS_DIR ${PROJECT_SOURCE_DIR}/Shared)
file(GLOB SHARED_HEADERS ${SHARED_HEADERS_DIR}/*.h)

add_executable(AuctionSvr ${AUCTION_SVR_HEADERS} ${AUCTION_SVR_SOURCES})
add_executable(AuthSvr ${AUTH_SVR_HEADERS} ${AUTH_SVR_SOURCES} ${SHARED_HEADERS})
add_executable(ChatSvr ${CHAT_SVR_HEADERS} ${CHAT_SVR_SOURCES})
add_library(CoreLib STATIC ${CORE_LIB_HEADERS} ${CORE_LIB_SOURCES})
add_executable(MasterSvr ${MASTER_SVR_HEADERS} ${MASTER_SVR_SOURCES} ${SHARED_HEADERS})
add_library(NetLib STATIC ${NET_LIB_HEADERS} ${NET_LIB_SOURCES})
add_library(RuntimeDataLib STATIC ${RUNTIME_DATA_LIB_HEADERS} ${RUNTIME_DATA_LIB_SOURCES})
add_library(RuntimeLib STATIC ${RUNTIME_LIB_HEADERS} ${RUNTIME_LIB_SOURCES})
add_executable(WorldSvr ${WORLD_SVR_HEADERS} ${WORLD_SVR_SOURCES} ${SHARED_HEADERS})

target_include_directories(AuctionSvr PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(AuthSvr PUBLIC ${PROJECT_SOURCE_DIR} ${OPENSSL_INCLUDE_DIR} ${SHARED_HEADERS_DIR})
target_include_directories(ChatSvr PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(CoreLib PUBLIC ${MARIADB_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
target_include_directories(MasterSvr PUBLIC ${PROJECT_SOURCE_DIR} ${SHARED_HEADERS_DIR})
target_include_directories(NetLib PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(RuntimeDataLib PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(RuntimeLib PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(WorldSvr PUBLIC ${PROJECT_SOURCE_DIR} ${SHARED_HEADERS_DIR})

target_link_libraries(AuctionSvr NetLib CoreLib)
target_link_libraries(AuthSvr NetLib CoreLib libcrypto_static libssl)
target_link_libraries(ChatSvr NetLib CoreLib)
target_link_libraries(CoreLib libmariadb zlib)
target_link_libraries(NetLib CoreLib)
target_link_libraries(MasterSvr NetLib CoreLib RuntimeLib RuntimeDataLib)
target_link_libraries(NetLib CoreLib)
target_link_libraries(RuntimeDataLib CoreLib)
target_link_libraries(RuntimeLib CoreLib RuntimeDataLib)
target_link_libraries(WorldSvr NetLib CoreLib RuntimeLib RuntimeDataLib)

if(WIN32)
  	target_link_libraries(NetLib wsock32 ws2_32)
endif()
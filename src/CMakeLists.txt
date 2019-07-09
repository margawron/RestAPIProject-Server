CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(SerwerREST)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#  Dodaj biblioteki boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
# Ścieżka do bibliotek boost
set(BOOST_ROOT /soft/libs/boost_1_70_0)
find_package(Boost 1.70.0 COMPONENTS system filesystem)
find_package(Threads)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(SOURCES
            connection.cpp
            database.cpp
            databaseadaptor.cpp
            exception.cpp
            http_codes.cpp
            iconnectionadaptor.cpp
            iserializable.cpp
            server.cpp
            user.cpp
            main.cpp
            )
    add_executable(Server ${SOURCES})
    target_link_libraries(Server ${Boost_LIBRARIES} Threads::Threads)
endif()

adasfasdfasfafa4f1234kj2345vuh235gr23g5r21g5hisdf12

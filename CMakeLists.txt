cmake_minimum_required(VERSION 3.10)


project(brainfucknt)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if(CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Ofast")
elseif (MSVC)
    set(CMAKE_CXX_FLAGS "-Ot -EHsc") # Why does windows have to be a nightmare
endif()

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
set(BOOST_REQUESTED_VERSION 1.61.0)
find_package(Boost 1.61.0 REQUIRED COMPONENTS system) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(brainfucknt main.cpp) 
    target_link_libraries(brainfucknt ${Boost_LIBRARIES})
endif()
message("What is wrong with you. Why do you do this?")
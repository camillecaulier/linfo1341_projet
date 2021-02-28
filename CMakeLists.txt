cmake_minimum_required(VERSION 3.17)
project(projet)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(projet
        src/log.c
        src/log.h
        src/receiver.c
        src/sender.c
        tests/run_tests.sh
        tests/simple_test.sh)

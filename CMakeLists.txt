cmake_minimum_required(VERSION 4.3)
project(elektronicky_zapisnik)

set(CMAKE_CXX_STANDARD 20)

add_executable(elektronicky_zapisnik src/main.cpp
        include/notebook.h
        include/storage.h
        include/ui.h
        src/storage.cpp
        src/ui.cpp)

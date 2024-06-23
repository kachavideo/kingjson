# CMakeLists.txt
cmake_minimum_required(VERSION 2.8)

project(kingJson)

add_executable(${PROJECT_NAME} 
                ../CKingJson.cpp
                ../CBangJson.cpp
                ../UTextFunc.cpp
                ./test.cpp
                ) 

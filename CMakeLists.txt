cmake_minimum_required(VERSION 2.6)
project(compilation)

enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O0 -g")

# Define where headers only libs and statics libs are located
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/boost)
include_directories(${CMAKE_SOURCE_DIR}/src)


add_subdirectory(src)

# Use Astar to reformat source files
add_custom_target(format
        COMMAND astyle -RA2fpHUxC90xLyjxek1W1 \"src/*.cpp\" \"src/*.hpp\"
        COMMAND rm -Rf src/*.orig
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

cmake_minimum_required(VERSION 2.6)
project(compilation)
set(BOOST_LIBS program_options)
set(LIBS_DIR third_party/boost/stage/lib/)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O0 -g -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O0 -g -std=gnu11")


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setenv.sh ${CMAKE_CURRENT_BINARY_DIR}/setenv.sh @ONLY)
# Define where headers only libs and statics libs are located
#include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/boost)
include_directories(${CMAKE_SOURCE_DIR}/src)
link_directories(${CMAKE_SOURCE_DIR}/${LIBS_DIR})


# Allow to automaticly unpack and build boost library according to build type
# It can build both libraries for classic compilation and cross compilation
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/third_party/boost)
    message(STATUS "Unpacking boost...")
    execute_process(COMMAND tar -xf boost.tar.bz2
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party)
    execute_process(COMMAND mv boost_1_62_0 boost
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party)
    message(STATUS "done")
else ()
    message(STATUS "Boost directory found")
endif ()

# Build required libs
foreach (BOOST_LIB ${BOOST_LIBS})
    if (NOT EXISTS ${CMAKE_SOURCE_DIR}/third_party/boost/stage/lib/libboost_${BOOST_LIB}.a)
        message(STATUS "${BOOST_LIB} missing")
        message(STATUS "Building Boost required library...")
        execute_process(COMMAND sh bootstrap.sh --with-libraries=${BOOST_LIB}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/boost)
        execute_process(COMMAND ./b2 link=static
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/boost)
    message(STATUS "done")
    endif ()
endforeach ()


add_subdirectory(src)
add_subdirectory(test)

# Use Astyle to reformat source files
add_custom_target(format
        COMMAND astyle -RA2fpHUxC200xLyjxek1W1 \"src/*.cpp\" \"src/*.hpp\"
        COMMAND rm -Rf src/*.orig
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

add_custom_target(version
        COMMAND clang --version)

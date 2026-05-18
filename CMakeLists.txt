cmake_minimum_required(VERSION 3.14)
project("GunningUp")


# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 6.0
        GIT_SHALLOW 1
    )
    FetchContent_MakeAvailable(raylib)
endif()

# raylib-cpp
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    if (NOT DEFINED RAYLIB_CPP_VERSION)
        set(RAYLIB_CPP_VERSION v6.0.0)
    endif()
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG ${RAYLIB_CPP_VERSION}
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

add_executable(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Collect source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Collect header files
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

#create the source for the game
target_sources(${PROJECT_NAME} 
PRIVATE
    # ALL SOURCES GO HERE
        ${SOURCE_FILES}
PUBLIC
    FILE_SET myHeaders
    TYPE HEADERS
    BASE_DIRS
        include
    FILES
    # ALL HEADERS GO HERE
        ${HEADER_FILES}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${SOURCE_FILES} ${HEADER_FILES}
)

target_link_libraries(${PROJECT_NAME} 
PUBLIC 
    raylib raylib_cpp
)

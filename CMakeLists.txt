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
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

# Fetch GoogleTest framework
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# Prevent overriding parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable CTest
enable_testing()

# Define your test executable
add_executable(my_tests GTesting/test.cpp)
target_link_libraries(my_tests 
    GTest::gtest_main
    raylib raylib_cpp
)

# Automatically discover tests from the binary
include(GoogleTest)
gtest_discover_tests(my_tests)

# Collect source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/GunningUp/src/*.cpp"
)

# Collect header files
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/GunningUp/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/GunningUp/include/*.hpp"
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
        GunningUp/include
    FILES
    # ALL HEADERS GO HERE
        ${HEADER_FILES}
)

target_link_libraries(${PROJECT_NAME} 
PUBLIC 
    raylib raylib_cpp
)

list(REMOVE_ITEM SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/GunningUp/src/main.cpp")
#create the source for the game
target_sources(my_tests
PRIVATE
    # ALL SOURCES GO HERE
        ${SOURCE_FILES}
PUBLIC
    FILE_SET myHeaders
    TYPE HEADERS
    BASE_DIRS
        GunningUp/include
    FILES
    # ALL HEADERS GO HERE
        ${HEADER_FILES}
)


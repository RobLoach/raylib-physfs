cmake_minimum_required(VERSION 3.11)
project (raylib-physfs
    VERSION 4.0.0
    DESCRIPTION "raylib-physfs"
    HOMEPAGE_URL "https://github.com/robloach/raylib-physfs"
    LANGUAGES C)

# Include Directory
add_subdirectory(include)

# Static Library
option(BUILD_RAYLIB_PHYSFS_LIB "Static Library" ON)
if(BUILD_RAYLIB_PHYSFS_LIB)
    add_subdirectory(lib)
endif()

# Examples
option(BUILD_RAYLIB_PHYSFS_EXAMPLES "Examples" ON)
if(BUILD_RAYLIB_PHYSFS_EXAMPLES)
    add_subdirectory(examples)
endif()

# Testing
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    enable_testing()
    if(BUILD_TESTING AND BUILD_RAYLIB_PHYSFS_LIB)
        add_subdirectory(test)
    endif()
endif()

cmake_minimum_required(VERSION 3.11)
project (raylib-physfs
    VERSION 1.3.0
    DESCRIPTION "raylib-physfs"
    HOMEPAGE_URL "https://github.com/robloach/raylib-physfs"
    LANGUAGES C)

# Include Directory
add_subdirectory(include)

# Examples
option(BUILD_RAYLIB_PHYSFS_EXAMPLES "Examples" ON)
if(BUILD_RAYLIB_PHYSFS_EXAMPLES)
    add_subdirectory(examples)
endif()

# Testing
include(CTest)
enable_testing()
if(BUILD_TESTING AND BUILD_RAYLIB_PHYSFS_EXAMPLES)
    set(CTEST_CUSTOM_TESTS_IGNORE
        pkg-config--static
    )
    add_subdirectory(test)
endif()

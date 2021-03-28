cmake_minimum_required(VERSION 3.11)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
project (raylib-physfs
    VERSION 0.0.1
    DESCRIPTION "raylib-physfs"
    HOMEPAGE_URL "https://github.com/robloach/raylib-physfs"
    LANGUAGES C)

# Options
option(BUILD_RAYLIB_PHYSFS_EXAMPLES "Examples" ON)

# Include Directory
add_subdirectory(include)

# Examples
if(BUILD_RAYLIB_CPP_EXAMPLES)
    add_subdirectory(examples)
endif()

# Testing
include(CTest)
enable_testing()
if(BUILD_TESTING)
    set(CTEST_CUSTOM_TESTS_IGNORE
        pkg-config--static
    )

    add_subdirectory(tests)
endif()
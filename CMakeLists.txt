cmake_minimum_required(VERSION 3.11)
project (raylib-physfs
    VERSION 6.0.0
    DESCRIPTION "raylib-physfs"
    HOMEPAGE_URL "https://github.com/robloach/raylib-physfs"
    LANGUAGES C
)

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(RAYLIB_PHYSFS_IS_MAIN TRUE)
else()
    set(RAYLIB_PHYSFS_IS_MAIN FALSE)
endif()
option(RAYLIB_PHYSFS_BUILD_EXAMPLES "Examples" ${RAYLIB_PHYSFS_IS_MAIN})
option(RAYLIB_PHYSFS_PLATFORM_RAYLIB "Use raylib as the PhysFS platform layer instead of the default OS platform" OFF)

# Library
add_library(raylib_physfs INTERFACE)

# Include Directory
target_include_directories(raylib_physfs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)

# Propagate PHYSFS_PLATFORM_RAYLIB to all consumers of this library.
if (RAYLIB_PHYSFS_PLATFORM_RAYLIB)
    target_compile_definitions(raylib_physfs INTERFACE PHYSFS_PLATFORM_RAYLIB)
endif()

# Install the header and the raylib platform implementation together.
install(FILES raylib-physfs.h physfs_platform_raylib.c
    DESTINATION include
)

# examples
if (RAYLIB_PHYSFS_BUILD_EXAMPLES)
    add_subdirectory(examples)

    # Testing
    include(CTest)
    enable_testing()
    if(BUILD_TESTING)
        # set(CTEST_CUSTOM_TESTS_IGNORE
        #     pkg-config--static
        # )
        add_subdirectory(test)
    endif()
endif()

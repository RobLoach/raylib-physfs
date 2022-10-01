cmake_minimum_required(VERSION 3.11)
project (raylib-physfs
    VERSION 4.2.1
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

# Library
add_library(raylib_physfs INTERFACE)

# Include Directory
target_include_directories(raylib_physfs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)

# Set the header files as install files.
install(FILES raylib-physfs.h
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

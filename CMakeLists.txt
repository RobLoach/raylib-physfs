cmake_minimum_required(VERSION 3.11)
project (raylib-physfs
    VERSION 4.0.2
    DESCRIPTION "raylib-physfs"
    HOMEPAGE_URL "https://github.com/robloach/raylib-physfs"
    LANGUAGES C
)

# Include Directory
add_subdirectory(include)

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(RAYLIB_PHYSFS_IS_MAIN TRUE)
else()
    set(RAYLIB_PHYSFS_IS_MAIN FALSE)
endif()
option(RAYLIB_PHYSFS_BUILD_EXAMPLES "Examples" ${RAYLIB_PHYSFS_IS_MAIN})

# Examples
if (RAYLIB_PHYSFS_BUILD_EXAMPLES)
    add_subdirectory(examples)

    # Testing
    if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        include(CTest)
        enable_testing()
        if (BUILD_TESTING)
            add_subdirectory(test)
        endif()
    endif()
endif()

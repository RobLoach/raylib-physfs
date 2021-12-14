# Get the sources together
set(example_dirs audio shaders text textures)
set(example_sources)
set(example_resources)

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # or games
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/raylib ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/raylib)
endif()

# PhysFS
find_package(physfs QUIET)
if (NOT physfs_FOUND)
    set(PHYSFS_BUILD_STATIC ON CACHE BOOL "TRUE" FORCE)
    set(PHYSFS_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(PTHREAD_LIBRARY OFF CACHE BOOL "" FORCE)
    set(PHYSFS_ARCHIVE_ZIP OFF CACHE BOOL "" FORCE) # Enable PHYSFS_ARCHIVE_ZIP for .zip support
    set(PHYSFS_ARCHIVE_7Z OFF CACHE BOOL "" FORCE)
    set(PHYSFS_ARCHIVE_GRP OFF CACHE BOOL "" FORCE)
    set(PHYSFS_ARCHIVE_WAD OFF CACHE BOOL "" FORCE)
    set(PHYSFS_ARCHIVE_HOG OFF CACHE BOOL "" FORCE)
    set(PHYSFS_ARCHIVE_MVL OFF CACHE BOOL "" FORCE)
    set(PHYSFS_ARCHIVE_QPAK OFF CACHE BOOL "" FORCE)
    set(PHYSFS_ARCHIVE_SLB OFF CACHE BOOL "" FORCE)
    set(PHYSFS_ARCHIVE_ISO9660 OFF CACHE BOOL "" FORCE)
    set(PHYSFS_ARCHIVE_VDF OFF CACHE BOOL "" FORCE)
    set(PHYSFS_BUILD_TEST OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/physfs ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/physfs)
    target_include_directories(physfs-static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/physfs/src)
endif()

# Find all examples
foreach(example_dir ${example_dirs})
    file(GLOB sources ${example_dir}/*.c)
    list(APPEND example_sources ${sources})

    # Any any resources
    file(GLOB resources ${example_dir}/resources/*)
    list(APPEND example_resources ${resources})
endforeach()

# Compile all examples
foreach(example_source ${example_sources})
    # Create the basename for the example
    get_filename_component(example_name ${example_source} NAME)
    string(REPLACE ".c" "${OUTPUT_EXT}" example_name ${example_name})

    # Setup the example
    add_executable(${example_name} ${example_source})

    # Link raylib and raylib-cpp
    target_link_libraries(${example_name} PUBLIC raylib physfs-static raylib-physfs)

    # Ensure physfs is included.
    target_include_directories(${example_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/physfs/src)

    string(REGEX MATCH ".*/.*/" resources_dir ${example_source})
    string(APPEND resources_dir "resources")
endforeach()

# Copy all the resources
file(COPY ${example_resources} DESTINATION "resources/")

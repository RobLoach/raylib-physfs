# Get the sources together
set(example_dirs audio shaders text textures)
set(example_sources)
set(example_resources)

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
        FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.0.0
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
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
    target_link_libraries(${example_name} PUBLIC raylib raylib_physfs)

    target_compile_definitions(${example_name} PUBLIC
        PHYSFS_SUPPORTS_NO_ZIP
        PHYSFS_SUPPORTS_NO_7Z
        PHYSFS_SUPPORTS_NO_GRP
        PHYSFS_SUPPORTS_NO_WAD
        PHYSFS_SUPPORTS_NO_HOG
        PHYSFS_SUPPORTS_NO_MVL
        PHYSFS_SUPPORTS_NO_QPAK
        PHYSFS_SUPPORTS_NO_SLB
        PHYSFS_SUPPORTS_NO_ISO9660
        PHYSFS_SUPPORTS_NO_VDF
    )

    string(REGEX MATCH ".*/.*/" resources_dir ${example_source})
    string(APPEND resources_dir "resources")
endforeach()

# Copy all the resources
file(COPY ${example_resources} DESTINATION "resources/")

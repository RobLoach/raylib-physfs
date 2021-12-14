add_library(raylib-physfs INTERFACE)

# Include Directory
target_include_directories(raylib-physfs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)

# Options
option(RAYLIB_PHYSFS_STATIC "Static Definition" OFF)
if (RAYLIB_PHYSFS_STATIC)
    target_compile_definitions(raylib-physfs INTERFACE RAYLIB_PHYSFS_STATIC)
endif()

# Set the header files as install files.
install(FILES raylib-physfs.h miniphysfs.h
    DESTINATION include
)

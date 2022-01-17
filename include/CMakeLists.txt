add_library(raylib_physfs INTERFACE)

# Include Directory
target_include_directories(raylib_physfs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)

# Set the header files as install files.
install(FILES raylib-physfs.h miniphysfs.h
    DESTINATION include
)

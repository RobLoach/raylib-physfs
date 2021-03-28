add_library(raylib-physfs INTERFACE)

# Include Directory
target_include_directories(raylib-physfs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)

# Set the header files as install files.
install(FILES
    raylib-physfs.h
    DESTINATION include
)

# Static Library
add_library(raylib-physfs-static STATIC raylib-physfs.c)
target_link_libraries(raylib-physfs-static PUBLIC raylib raylib-physfs)
#target_compile_definitions(raylib-physfs-static PUBLIC RAYLIB_PHYSFS_STATIC)

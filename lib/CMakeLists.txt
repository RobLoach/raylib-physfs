# Static Library
add_library(raylib-physfs-static STATIC raylib-physfs.c)
target_link_libraries(raylib-physfs-static PUBLIC raylib_static raylib-physfs)
#target_compile_definitions(raylib-physfs-static PUBLIC RAYLIB_PHYSFS_STATIC)

# Shared Library
#add_library(raylib-physfs-shared SHARED raylib-physfs.c)
#target_link_libraries(raylib-physfs-shared PUBLIC raylib raylib-physfs)
#target_compile_definitions(raylib-physfs-static PUBLIC RAYLIB_PHYSFS_STATIC)

cmake_minimum_required(VERSION 3.11)
project (raylib-physfs
    VERSION 4.0.2
    DESCRIPTION "raylib-physfs"
    HOMEPAGE_URL "https://github.com/robloach/raylib-physfs"
    LANGUAGES C
)

include(FetchContent)

# Adding Raylib for graphics
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG master)
FetchContent_MakeAvailable(raylib)

# Adding PhysFS for filesystem abstraction
# You can change these to support other formats
set(PHYSFS_ARCHIVE_ZIP ON CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_7Z OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_GRP OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_WAD OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_HOG OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_MVL OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_QPAK OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_SLB OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_ISO9660 OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_VDF OFF CACHE BOOL "" FORCE)

# library options
set(PHYSFS_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(PHYSFS_DISABLE_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(physfs GIT_REPOSITORY https://github.com/icculus/physfs.git GIT_TAG main)
FetchContent_MakeAvailable(physfs)
include_directories(${physfs_SOURCE_DIR}/src)

# demos

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples/audio/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples/shaders/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples/text/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples/textures/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

add_executable(audio_music_stream)
target_sources(audio_music_stream PRIVATE "${CMAKE_CURRENT_LIST_DIR}/examples/audio/audio_music_stream.c")
target_include_directories(audio_music_stream PRIVATE ".")
target_link_libraries(audio_music_stream PRIVATE raylib physfs-static)

add_executable(audio_sound_loading)
target_sources(audio_sound_loading PRIVATE "${CMAKE_CURRENT_LIST_DIR}/examples/audio/audio_sound_loading.c")
target_include_directories(audio_sound_loading PRIVATE ".")
target_link_libraries(audio_sound_loading PRIVATE raylib physfs-static)

add_executable(shaders_texture_waves)
target_sources(shaders_texture_waves PRIVATE "${CMAKE_CURRENT_LIST_DIR}/examples/shaders/shaders_texture_waves.c")
target_include_directories(shaders_texture_waves PRIVATE ".")
target_link_libraries(shaders_texture_waves PRIVATE raylib physfs-static)

add_executable(text_basic_loading)
target_sources(text_basic_loading PRIVATE "${CMAKE_CURRENT_LIST_DIR}/examples/text/text_basic_loading.c")
target_include_directories(text_basic_loading PRIVATE ".")
target_link_libraries(text_basic_loading PRIVATE raylib physfs-static)

add_executable(text_font_loading)
target_sources(text_font_loading PRIVATE "${CMAKE_CURRENT_LIST_DIR}/examples/text/text_font_loading.c")
target_include_directories(text_font_loading PRIVATE ".")
target_link_libraries(text_font_loading PRIVATE raylib physfs-static)

add_executable(textures_image_loading)
target_sources(textures_image_loading PRIVATE "${CMAKE_CURRENT_LIST_DIR}/examples/textures/textures_image_loading.c")
target_include_directories(textures_image_loading PRIVATE ".")
target_link_libraries(textures_image_loading PRIVATE raylib physfs-static)


# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.2.0
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

# physfs
find_package(physfs QUIET)
if (NOT physfs_FOUND)
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

    include(FetchContent)
    FetchContent_Declare(
        physfs
        GIT_REPOSITORY https://github.com/icculus/physfs.git
        GIT_TAG release-3.2.0
    )
    FetchContent_MakeAvailable(physfs)
    include_directories(${physfs_SOURCE_DIR}/src)
endif()

# Resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/audio/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/text/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/textures/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

add_executable(audio_music_stream)
target_sources(audio_music_stream PRIVATE "${CMAKE_CURRENT_LIST_DIR}/audio/audio_music_stream.c")
target_link_libraries(audio_music_stream PRIVATE raylib physfs-static raylib_physfs)

add_executable(audio_sound_loading)
target_sources(audio_sound_loading PRIVATE "${CMAKE_CURRENT_LIST_DIR}/audio/audio_sound_loading.c")
target_link_libraries(audio_sound_loading PRIVATE raylib physfs-static raylib_physfs)

add_executable(shaders_texture_waves)
target_sources(shaders_texture_waves PRIVATE "${CMAKE_CURRENT_LIST_DIR}/shaders/shaders_texture_waves.c")
target_link_libraries(shaders_texture_waves PRIVATE raylib physfs-static raylib_physfs)

add_executable(text_basic_loading)
target_sources(text_basic_loading PRIVATE "${CMAKE_CURRENT_LIST_DIR}/text/text_basic_loading.c")
target_link_libraries(text_basic_loading PRIVATE raylib physfs-static raylib_physfs)

add_executable(text_font_loading)
target_sources(text_font_loading PRIVATE "${CMAKE_CURRENT_LIST_DIR}/text/text_font_loading.c")
target_link_libraries(text_font_loading PRIVATE raylib physfs-static raylib_physfs)

add_executable(textures_image_loading)
target_sources(textures_image_loading PRIVATE "${CMAKE_CURRENT_LIST_DIR}/textures/textures_image_loading.c")
target_link_libraries(textures_image_loading PRIVATE raylib physfs-static raylib_physfs)

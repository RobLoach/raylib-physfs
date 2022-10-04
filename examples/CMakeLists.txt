
# Add the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

find_package(Raylib)
find_package(PhysFS)

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

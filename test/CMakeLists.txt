# raylib-physfs-test
add_executable(raylib-physfs-test raylib-physfs-test.c)
target_link_libraries(raylib-physfs-test PUBLIC
    raylib
    raylib-physfs-static
)

# Copy the resources
file(GLOB resources resources/*)
set(test_resources)
list(APPEND test_resources ${resources})
file(COPY ${test_resources} DESTINATION "resources/")

# Set up the test
add_test(NAME raylib-physfs-test COMMAND raylib-physfs-test)

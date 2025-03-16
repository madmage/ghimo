cmake_minimum_required(VERSION 3.16)
project(ghimo)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

file(GLOB_RECURSE CPP_SOURCES src/*.cpp)
add_library(ghimo ${CPP_SOURCES})
target_include_directories(ghimo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS ghimo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)

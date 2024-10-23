cmake_minimum_required(VERSION 3.29)
project(easySTL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(easySTL LANGUAGES CXX)

set(SOURCES src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Catch2 -- Unit test
find_package(Catch2 3 REQUIRED)
set(TEST_SOURCES test/allocator_test.cpp
        test/iterator_test.cpp
        test/constructor_test.cpp
        test/uninitialized_test.cpp
        test/algo_test.cpp
        test/vector_test.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Add compile flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    target_compile_options(tests PRIVATE /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion)
    target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion)
endif ()
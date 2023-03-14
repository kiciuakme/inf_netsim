cmake_minimum_required(VERSION 3.13)
project(NetSim)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors)

include_directories(include)

# Utwórz zmienną `SOURCE_FILES` przechowującą ścieżki do plików źródłowych.
set(SOURCE_FILES
        src/package.cpp src/storage_types.cpp src/nodes.cpp src/helpers.cpp src/factory.cpp)

# Uwzględnij zawartość zmiennej `SOURCE_FILES` w procesie kompilacji.
add_executable(NetSim ${SOURCE_FILES} main.cpp
        include/factory.hpp
        include/helpers.hpp
        include/nodes.hpp
        include/package.hpp
        include/reports.hpp
        include/simulation.hpp
        include/storage_types.hpp
        include/types.hpp)

set(SOURCES_FILES_TESTS
        test/package_tests.cpp
        )

add_executable(NetSim__test ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_gtest.cpp)


target_include_directories(NetSim__test PUBLIC
        googletest-master/googlemock/include
        googletest-master/googletest/include
        )

add_subdirectory(googletest-master googletest-master)

target_link_libraries(NetSim__test gmock)

cmake_minimum_required(VERSION 3.16)
project(6g_ran_edge_lab VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ranedge
    src/anomaly_detector.cpp
    src/cell.cpp
    src/scheduler.cpp
    src/simulation.cpp
)

target_include_directories(ranedge PUBLIC include)
target_compile_options(ranedge PRIVATE -Wall -Wextra -Wpedantic)

add_executable(ranedge-sim src/main.cpp)
target_link_libraries(ranedge-sim PRIVATE ranedge)

enable_testing()
add_executable(ranedge_tests
    tests/test_anomaly_detector.cpp
    tests/test_scheduler.cpp
    tests/test_simulation.cpp
)
target_link_libraries(ranedge_tests PRIVATE ranedge)
target_compile_options(ranedge_tests PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME ranedge_tests COMMAND ranedge_tests)

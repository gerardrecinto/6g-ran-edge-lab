cmake_minimum_required(VERSION 3.16)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" RANEDGE_VERSION_FILE)
string(STRIP "${RANEDGE_VERSION_FILE}" RANEDGE_VERSION_FILE)

project(6g_ran_edge_lab VERSION ${RANEDGE_VERSION_FILE} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ranedge
    src/anomaly_detector.cpp
    src/cell.cpp
    src/scheduler.cpp
    src/simulation.cpp
    src/telemetry.cpp
)

target_include_directories(ranedge PUBLIC include)
target_compile_definitions(ranedge PRIVATE RANEDGE_VERSION="${PROJECT_VERSION}")
target_compile_options(ranedge PRIVATE -Wall -Wextra -Wpedantic)

add_executable(ranedge-sim src/main.cpp)
target_link_libraries(ranedge-sim PRIVATE ranedge)

enable_testing()

foreach(test_name
    test_anomaly_detector
    test_cell
    test_scheduler
    test_simulation
    test_telemetry
)
    add_executable(${test_name} tests/${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE ranedge)
    target_compile_options(${test_name} PRIVATE -Wall -Wextra -Wpedantic)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

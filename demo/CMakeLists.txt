cmake_minimum_required(VERSION 3.8)

project("codie-demo")

file(GLOB_RECURSE CODIE_DEMO_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.ixx)

add_executable(${PROJECT_NAME}
    ${CODIE_DEMO_SOURCES}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME}
    codie::json
)
cmake_minimum_required(VERSION 3.8)

project("codie")

file(GLOB_RECURSE CODIE_JSON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/json/*.ixx)

add_library("codie-json"
	${CODIE_JSON_SOURCES}
)

add_library(codie::json ALIAS codie-json)

target_compile_features("codie-json" PRIVATE cxx_std_20)

add_subdirectory(demo)
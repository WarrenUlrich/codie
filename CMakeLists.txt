# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("codie")

file(GLOB_RECURSE CODIE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.ixx)

add_library(${PROJECT_NAME}
	${CODIE_SOURCES}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

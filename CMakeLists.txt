cmake_minimum_required(VERSION 3.8)

project("codie")

file(GLOB_RECURSE CODIE_JSON_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/src/json/*.ixx)

# file(GLOB_RECURSE CODIE_CSV_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/src/csv/*.ixx)
add_library("codie-json"
	${CODIE_JSON_MODULES}
)

target_compile_features("codie-json" PRIVATE cxx_std_20)

# add_library("codie-csv"
# ${CODIE_CSV_MODULES}
# )

# target_compile_features("codie-csv" PRIVATE cxx_std_20)
add_library(codie::json ALIAS codie-json)

# add_library(codie::csv ALIAS codie-csv)
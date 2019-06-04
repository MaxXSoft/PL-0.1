cmake_minimum_required(VERSION 3.0)
project(PL/0.1 VERSION "0.0.1")

# # set compiler path
# set(CMAKE_C_COMPILER "/usr/local/opt/gcc/bin/gcc-8")
# set(CMAKE_CXX_COMPILER "/usr/local/opt/gcc/bin/g++-8")

# C++17 standard support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# enable all warnings and treat them as errors
if(MSVC)
  add_compile_options(/W3 /WX)
else()
  add_compile_options(-Wall -Werror)
endif()

# some definitions
add_compile_definitions(APP_NAME="${PROJECT_NAME}")
add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")
add_compile_definitions(APP_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_compile_definitions(APP_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_compile_definitions(APP_VERSION_PATCH=${PROJECT_VERSION_PATCH})

# find LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# project include directories
include_directories(src/inc)
include_directories(test)
include_directories(lib)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# PL/0.1 source files
file(GLOB_RECURSE PL01_SRC "src/*.cpp")

# source files exclude main driver
set(BASIC_SRC ${PL01_SRC})
list(REMOVE_ITEM BASIC_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# unit tests' source files
file(GLOB_RECURSE TEST_SRC "test/*.cpp")

# PL/0.1 runtime library source files
file(GLOB_RECURSE LIB_SRC "lib/*.c")

# compiler course design
set(LAB_SRC1 "lab/lexer_test.cpp")
set(LAB_SRC2 "lab/highlight.cpp")
set(LAB_SRC3 "lab/parser_test.cpp")

# create executable files
add_executable(pl01 ${PL01_SRC})
add_executable(test ${BASIC_SRC} ${TEST_SRC})
add_library(pl01rt ${LIB_SRC})
add_executable(lexer_test "src/front/lexer.cpp" ${LAB_SRC1})
add_executable(highlight "src/front/lexer.cpp" ${LAB_SRC2})
add_executable(parser_test ${BASIC_SRC} ${LAB_SRC3})

# find LLVM libraries & link
llvm_map_components_to_libnames(LLVM_LIBS all)
target_link_libraries(pl01 ${LLVM_LIBS})
target_link_libraries(test ${LLVM_LIBS})
target_link_libraries(test pl01rt)
target_link_libraries(parser_test ${LLVM_LIBS})

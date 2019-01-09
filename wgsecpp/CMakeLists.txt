cmake_minimum_required(VERSION 3.0)


project(wgsecpp)


# Source files to be used in the library
set(library_HEADERS
	include/types.hpp
	include/binding.hpp
)

set(library_SOURCES
	src/wgsecpp.cpp
)

add_library(wgsecpp ${library_SOURCES} ${library_HEADERS} )

target_link_libraries(wgsecpp d3d11 d3dcompiler dxguid)
target_include_directories(wgsecpp
	PUBLIC ${PROJECT_SOURCE_DIR}/include
)
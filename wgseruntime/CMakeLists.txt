cmake_minimum_required(VERSION 3.0)


project(wgseruntime)


# Source files to be used in the library
set(library_HEADERS
	include/opcodes.h
	include/vm.h
	include/types.h
)

set(library_SOURCES
	src/vm.c
)

add_library(wgseruntime ${library_SOURCES} ${library_HEADERS} )

target_link_libraries(wgseruntime d3d11 d3dcompiler dxguid)
target_include_directories(wgseruntime
	PUBLIC ${PROJECT_SOURCE_DIR}/include
)
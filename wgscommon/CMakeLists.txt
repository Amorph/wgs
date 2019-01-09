cmake_minimum_required(VERSION 3.0)


project(wgscommon)


# Source files to be used in the library
set(library_HEADERS
	include/wgscommon/types.h
	include/wgscommon/utf8.h
	include/wgscommon/wgs_core.h
	include/wgscommon/wgs_file.h
	include/wgscommon/wgs_file_tokenizer.h
	include/wgscommon/wgs_node.h
	include/wgscommon/wgs_node_list.h
	include/wgscommon/wgs_node_tree.h
)
set(library_SOURCES
	src/osfs.c
	src/osmem.c
	src/osnet.c
	src/utf8.c
	src/wgs_core.c
	src/wgs_file.c
	src/wgs_file_tokenizer.c
)

add_library(wgscommon STATIC ${library_SOURCES} ${library_HEADERS})

target_include_directories(wgscommon
	PUBLIC ${PROJECT_SOURCE_DIR}/include
)

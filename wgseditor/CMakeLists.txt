cmake_minimum_required(VERSION 3.0)


project(wgseditor)


# Source files to be used in the library
set(library_HEADERS
	include/editor.h
	include/font.h
	include/node.h
	include/unit.h
	include/utils.h
)

set(library_SOURCES
	src/editor.cpp
	src/font.cpp
	src/main.cpp
	src/node.cpp
	src/unit.cpp
	src/utils.cpp
)

set(library_IMGUI_HEADERS
	thirdparty/imgui/imgui.h
	thirdparty/imgui/misc/cpp/imgui_stdlib.h
	thirdparty/imgui/examples/imgui_impl_dx11.h
	thirdparty/imgui/examples/imgui_impl_win32.h
)

set(library_IMGUI_SOURCES
	thirdparty/imgui/imgui.cpp
	thirdparty/imgui/imgui_widgets.cpp
	thirdparty/imgui/imgui_demo.cpp
	thirdparty/imgui/imgui_draw.cpp
	thirdparty/imgui/misc/cpp/imgui_stdlib.cpp
	thirdparty/imgui/examples/imgui_impl_dx11.cpp
	thirdparty/imgui/examples/imgui_impl_win32.cpp
)

source_group("imgui"     FILES ${library_IMGUI_HEADERS} ${library_IMGUI_SOURCES})

add_executable(wgseditor ${library_SOURCES} ${library_HEADERS} ${library_IMGUI_HEADERS} ${library_IMGUI_SOURCES})

target_link_libraries(wgseditor wgseruntime wgscommon d3d11 d3dcompiler dxguid)
target_include_directories(wgseditor
	PUBLIC ${PROJECT_SOURCE_DIR}/include
	PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/imgui/
	PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/imgui/misc/cpp
	PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/imgui/examples
)
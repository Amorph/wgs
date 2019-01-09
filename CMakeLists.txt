cmake_minimum_required(VERSION 3.0)
project(wgse)

add_subdirectory(wgscommon)
add_subdirectory(wgseruntime)
add_subdirectory(wgsecpp)
add_subdirectory(wgseditorlib)
add_subdirectory(wgseprotocol)

add_subdirectory(wgseditor)
add_subdirectory(wgsepyruntime)

add_subdirectory(examples)
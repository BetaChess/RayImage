cmake_minimum_required(VERSION 3.20)

project(rayimage)

set(FLOWFORGELIB_PATH ${PROJECT_SOURCE_DIR}/vendor/FlowForge)

# add subdirectories
add_subdirectory(vendor/FlowForge)

add_subdirectory(src)
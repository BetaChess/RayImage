cmake_minimum_required(VERSION 3.20)

set(SOURCES
        pch.hpp
        main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Precompiled header
target_precompile_headers(${PROJECT_NAME}
        PUBLIC pch.hpp
)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(${PROJECT_NAME}
        PUBLIC ../vendor/FlowForge/src
)

target_link_directories(${PROJECT_NAME}
        PRIVATE ../vendor/FlowForge
)

target_link_libraries(${PROJECT_NAME}
        flowforge_lib
)

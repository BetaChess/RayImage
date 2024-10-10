cmake_minimum_required(VERSION 3.20)

set(SOURCES
        pch.hpp
        main.cpp
        application.hpp
        application.cpp
        renderer.hpp
        renderer.cpp
        math/spectrum.hpp
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
        PUBLIC ${FLOWFORGELIB_PATH}/src
)

target_link_directories(${PROJECT_NAME}
        PRIVATE ${FLOWFORGELIB_PATH}
)

target_link_libraries(${PROJECT_NAME}
        flowforge_lib
)

add_shaders(${PROJECT_NAME}_shaders
        ${FLOWFORGELIB_PATH}/assets/shaders/default_material_shader.vert
        ${FLOWFORGELIB_PATH}/assets/shaders/default_material_shader.frag
)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(nandy2024)

include(cmake/CPM.cmake)

find_package(OpenGL REQUIRED)

CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 10.1.1
)

CPMAddPackage(
    NAME spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.12.0
    OPTIONS
        "SPDLOG_ENABLE_PCH ON"
        "SPDLOG_FMT_EXTERNAL ON"
)

CPMAddPackage(
    NAME SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.30.1
)

CPMAddPackage(
    NAME imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    # docking branch
    GIT_TAG 40df3db1a2a0f034257efad3c817c7ead5930257
)
if(imgui_ADDED)
    add_library(imgui
        STATIC
        # core
        ${imgui_SOURCE_DIR}/imconfig.h
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui.h
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_internal.h
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imstb_rectpack.h
        ${imgui_SOURCE_DIR}/imstb_textedit.h
        ${imgui_SOURCE_DIR}/imstb_truetype.h
        # backends
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.h
    )
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
    target_link_libraries(imgui PUBLIC SDL2::SDL2-static OpenGL::GL)
endif()

CPMAddPackage(
    NAME stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG master
)
if(stb_ADDED)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()

CPMAddPackage(
    NAME json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG develop
)

set(SOURCES
    src/app.cpp
    src/app.h
    src/common.h
    src/config.h
    src/main.cpp
    src/utils.h

    src/ui/renderer.cpp
    src/ui/renderer.h

    src/logic/gates.h
    src/logic/logic_sim.cpp
    src/logic/logic_sim.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} fmt::fmt spdlog SDL2::SDL2-static imgui stb nlohmann_json)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/logic
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

message(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_GENERATOR_PLATFORM: ${CMAKE_GENERATOR_PLATFORM}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

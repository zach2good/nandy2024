cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(nandy2024)

include(cmake/CPM.cmake)

CPMAddPackage(
    NAME spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.x
)

CPMAddPackage(
    NAME SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG SDL2
)

CPMAddPackage(
    NAME imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG docking
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
    target_link_libraries(imgui PUBLIC SDL2::SDL2-static opengl32)
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

set(SOURCES
    src/app.cpp
    src/app.h
    src/common.h
    src/config.h
    src/main.cpp
    src/utils.h

    src/ui/renderer.cpp
    src/ui/renderer.h
    src/ui/ui.cpp
    src/ui/ui.h

    src/logic/gates.h
    src/logic/logic_sim.cpp
    src/logic/logic_sim.h
)

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
target_link_libraries(${PROJECT_NAME} spdlog SDL2::SDL2-static opengl32 imgui stb)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logic
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

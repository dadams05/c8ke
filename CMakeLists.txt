cmake_minimum_required(VERSION 3.21)
project(c8ke LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# get dependencies via vcpkg
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)

# get dependencies via cmake
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    GIT_TAG "v1.92.5"
)
FetchContent_MakeAvailable(imgui)

add_library(imgui 
    STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(imgui
    PUBLIC
        "${imgui_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}/backends"
)
target_link_libraries(imgui
    PUBLIC
        SDL3::SDL3
)

# main executable
add_executable(c8ke
    "src/c8ke.cpp"
    "src/tinyfiledialogs.c"
    "src/gui.cpp"
    "src/main.cpp"
)
target_include_directories(c8ke
    PRIVATE
        "include"
)
target_link_libraries(c8ke
    PRIVATE
        SDL3_image::SDL3_image
        imgui
)

# post build commands
add_custom_command(TARGET c8ke POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
       $<TARGET_RUNTIME_DLLS:c8ke>
       $<TARGET_FILE_DIR:c8ke>
    COMMAND_EXPAND_LISTS
)
add_custom_command(TARGET c8ke POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/res"
        $<TARGET_FILE_DIR:c8ke>/res
)

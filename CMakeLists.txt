cmake_minimum_required(VERSION 3.21)
project(c8ke LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg dependencies
find_package(SDL3 CONFIG REQUIRED)

# cmake dependencies
include(FetchContent)
FetchContent_Declare(
    ImGui
    GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    GIT_TAG "v1.92.5"
)
FetchContent_MakeAvailable(ImGui)

# create the imgui library
add_library(ImGui 
    STATIC
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp"
)
target_include_directories(ImGui
    PUBLIC
        "${imgui_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}/backends"
)
target_link_libraries(ImGui
    PUBLIC
        SDL3::SDL3
)

# main executable
add_executable(c8ke
    "src/main.cpp"
    "src/c8ke.cpp"
    "src/gui.cpp"
    "src/tinyfiledialogs.cpp"
    "src/RobotoMono-Regular.cpp"
    "c8ke.rc"
)
target_include_directories(c8ke
    PRIVATE
        "include"
)
target_link_libraries(c8ke
    PRIVATE
        ImGui
)
# console in debug, GUI in release
set_target_properties(c8ke PROPERTIES
    WIN32_EXECUTABLE $<$<CONFIG:Release>:ON>
)

# post-build commands
add_custom_command(TARGET c8ke POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
       "$<TARGET_RUNTIME_DLLS:c8ke>"
       "$<TARGET_FILE_DIR:c8ke>"
    COMMAND_EXPAND_LISTS
)

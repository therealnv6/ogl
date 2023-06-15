cmake_minimum_required(VERSION 3.13)
project(meowfu)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)
set(IMGUI_LIBRARIES ImGui;glfw;GLEW;GL)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(spdlog REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/entt)
add_subdirectory(thirdparty/imterm)

find_program(MOLD_LINKER_EXE mold)
set(IMGUI_DIR thirdparty/imgui)

include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

if(MOLD_LINKER_EXE)
    # If mold is found, set the linker flags to use it
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=mold")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=mold")
else()
    # If mold is not found, print a warning message and use the default linker
    message(WARNING "mold linker not found on this system. Using default linker instead.")
endif()

file(GLOB_RECURSE SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/include/*.hpp
)

set(SOURCES 
  ${SOURCES}
  thirdparty/stb_image.h
)

set(IMGUI_SOURCES
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp 
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp 
  ${IMGUI_DIR}/imgui.cpp 
  ${IMGUI_DIR}/imgui_draw.cpp 
  ${IMGUI_DIR}/imgui_demo.cpp 
  ${IMGUI_DIR}/imgui_tables.cpp 
  ${IMGUI_DIR}/imgui_widgets.cpp
)

add_library(${PROJECT_NAME} STATIC ${IMGUI_SOURCES} ${SOURCES})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS}
    "thirdparty/glfw/include/"
    "thirdparty/imterm/include/"
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    EnTT::EnTT
    GLEW::GLEW ${GLEW_LIBRARIES} 
    ${OPENGL_LIBRARY} 
    spdlog::spdlog
)

IF (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE dbghelp)
ENDIF()

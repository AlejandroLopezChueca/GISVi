cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("GISVi" VERSION 1.0 LANGUAGES CXX C)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  add_compile_definitions(__OS_LINUX__)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
elseif ((${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  add_compile_definitions(__OS_WINDOWS__)
else()
  message("The Operating System is not found")
endif()


#option(USE_DEBUG "Debug mode" OFF)
set(USE_DEBUG ON CACHE BOOL "Debug mode" FORCE)

If(USE_DEBUG)
  add_definitions(-DDEBUG_MODE)
endif()

unset(USE_DEBUG CACHE)

###################################################################################################################
# MAIN
add_executable(${PROJECT_NAME} src/main.cpp)

#******************************************************************************************************************
# EXTERNAL LIBRARIES
#******************************************************************************************************************

###################################################################################################################
#SQLite3

#add_library(s_sqlite3)
#target_sources(s_sqlite3
#  PUBLIC
#    dependencies/sqlite3_3_43_1/shell.c
#    dependencies/sqlite3_3_43_1/sqlite3.c
#  PUBLIC
#    dependencies/sqlite3_3_43_1/sqlite3.h
#    dependencies/sqlite3_3_43_1/sqlite3ext.h)
#
#
#  #find_package(SQLite3)
#  #target_link_libraries(${PROJECT_NAME} PUBLIC SQLite3::SQLite3)
#
#target_link_libraries(${PROJECT_NAME} PUBLIC s_sqlite3)
###################################################################################################################
#PROJ

#find_package(GDAL CONFIG REQUIRED)
#add_subdirectory(dependencies/PROJ)
#find_package(PROJ REQUIRED CONFIG)
#add_compile_definitions(-DBUILD_APPS=OFF)
#add_compile_definitions(-DSQLITE3_INCLUDE_DIR=${CMAKE_SOURCE_DIR}/dependencies/sqlite3_3_43_1)

#target_link_libraries(${PROJECT_NAME} PUBLIC PROJ)
#target_link_libraries(${PROJECT_NAME} PUBLIC PROJ::proj)

###################################################################################################################
#GDAL

#find_package(GDAL CONFIG REQUIRED)
add_subdirectory(dependencies/gdal)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DBUILD_APPS=OFF)
target_link_libraries(${PROJECT_NAME} PRIVATE gdal)
#set_target_properties(gdal PROPERTIES -DBUILD_APPS = OFF)
#set_property(TARGET gdal PROPERTY -DBUILD_APPS = OFF)
#target_compile_definitions(gdal PUBLIC -DBUILD_APPS=OFF)
#target_link_libraries(${PROJECT_NAME} PRIVATE GDAL::GDAL)

###################################################################################################################
#GLFW

add_subdirectory(dependencies/glfw_3_3_8)
#Not build the GLFW tests, examples and documentation
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#target_include_directories(${PROJECT_NAME} PUBLIC dependencies/glfw_3_3_8/include)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

###################################################################################################################
#GLAD
add_library(s_glad)
target_sources(s_glad
  PUBLIC
    dependencies/glad/src/glad.c
    dependencies/glad/include/glad/glad.h)

target_include_directories(s_glad PUBLIC dependencies/glad/include)
target_link_libraries(${PROJECT_NAME} PUBLIC s_glad ${CMAKE_DL_LIBS})

###################################################################################################################
#STB
add_library(s_stb)
target_sources(s_stb
  PUBLIC
    dependencies/stb/stb_image.cpp
    dependencies/stb/stb_image.h)

target_include_directories(s_glad PUBLIC dependencies/stb)
target_link_libraries(${PROJECT_NAME} PUBLIC s_stb)

###################################################################################################################
#GLM
add_subdirectory(dependencies/glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)

###################################################################################################################
#ImGui
add_library(s_ImGui)
target_sources(s_ImGui
  PUBLIC
    dependencies/imgui/imgui.cpp
    dependencies/imgui/imgui_demo.cpp
    dependencies/imgui/imgui_draw.cpp
    dependencies/imgui/imgui_tables.cpp
    dependencies/imgui/imgui_widgets.cpp
    #backends
    dependencies/imgui/backends/imgui_impl_glfw.cpp
    dependencies/imgui/backends/imgui_impl_opengl3.cpp)

  target_include_directories(s_ImGui
    PUBLIC
      dependencies/imgui
      dependencies/imgui/backends
      dependencies/glfw_3_3_8/include)

target_link_libraries(${PROJECT_NAME} PUBLIC s_ImGui)

###################################################################################################################
#ImGuizmo
add_library(s_ImGuizmo)
target_sources(s_ImGuizmo
  PUBLIC
    dependencies/ImGuizmo/ImGuizmo.cpp
  PUBLIC
    dependencies/ImGuizmo/ImGuizmo.h
    )

  target_include_directories(s_ImGuizmo
    PUBLIC
      dependencies/imgui
      dependencies/ImGuizmo)

target_link_libraries(${PROJECT_NAME} PUBLIC s_ImGuizmo)

###################################################################################################################
#MSDF-ATLAS-GEM and MSDF

#set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "Build the msdf-atlas-gen standalone executable" FORCE)
#set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "Use vcpkg package manager to link project dependencies" FORCE)
#set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "Build with the Skia library" FORCE)
#set(MSDF_ATLAS_MSDFGEN_EXTERNAL OFF CACHE BOOL "Do not build the msdfgen submodule but find it as an external package" FORCE)

#set(MSDFGEN_BUILD_STANDALONE OFF CACHE BOOL "Build the msdfgen standalone executable" FORCE)
#set(MSDFGEN_USE_SKIA OFF CACHE BOOL "Build with the Skia library" FORCE)

# If it not required to lad svg files and saving PNG images
#set(MSDFGEN_DISABLE_SVG ON CACHE BOOL "" FORCE)
#set(MSDFGEN_DISABLE_PNG OFF CACHE BOOL "" FORCE)

#add_subdirectory(dependencies/msdf-atlas-gen)

#target_link_libraries(${PROJECT_NAME} PUBLIC msdf-atlas-gen)

###################################################################################################################
#Tiny File Dialogs
add_library(s_tiny)
target_sources(s_tiny
  PUBLIC
    dependencies/tinyfiledialogs/tinyfiledialogs.c
    dependencies/tinyfiledialogs/tinyfiledialogs.h)

target_include_directories(s_tiny PUBLIC dependencies/tinyfiledialogs)
target_link_libraries(${PROJECT_NAME} PUBLIC s_tiny)


#******************************************************************************************************************
# INTERNAL LIBRARIES
#******************************************************************************************************************

add_library(ui)
add_subdirectory(src/ui)
add_library(panels)
add_subdirectory(src/ui/panels)
add_library(io)
add_subdirectory(src/io)
add_library(main_src)
add_subdirectory(src)
add_library(graphics)
add_subdirectory(src/graphics)
add_library(OpenGL_graphics)
add_subdirectory(src/graphics/OpenGL)
add_library(scene)
add_subdirectory(src/graphics/scene)
add_library(math)
add_subdirectory(src/math)

include_directories(src)

target_link_libraries(${PROJECT_NAME} PUBLIC
  math
  io
  ui
  panels
  scene
  graphics
  OpenGL_graphics
  main_src)


#******************************************************************************************************************
# COPY ASSETS AND RESOURCES
#******************************************************************************************************************

add_custom_target(copy_assets
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets)

add_dependencies(${PROJECT_NAME} copy_assets)

add_custom_target(copy_resources
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources)

add_dependencies(${PROJECT_NAME} copy_resources)

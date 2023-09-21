cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("GISVi" VERSION 1.0 LANGUAGES CXX C)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  add_compile_definitions(__OS_LINUX__)
elseif ((${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  add_compile_definitions(__OS_WINDOWS__)
else()
  message("The Operating System is not found")
endif()

###################################################################################################################
# MAIN
add_executable(${PROJECT_NAME} src/main.cpp)

#******************************************************************************************************************
# EXTERNAL LIBRARIES
#******************************************************************************************************************

###################################################################################################################
#ImGui
#add_subdirectory(dependencies/fltk)
#target_link_libraries(${PROJECT_NAME} PUBLIC fltk)

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
target_link_libraries(${PROJECT_NAME} PUBLIC s_glad)

###################################################################################################################
#GLM
add_subdirectory(dependencies/glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)

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

#******************************************************************************************************************
# INTERNAL LIBRARIES
#******************************************************************************************************************

add_library(ui)
add_subdirectory(src/ui)
#add_library(io)
#add_subdirectory(src/io)
add_library(main_src)
add_subdirectory(src)
add_library(graphics)
add_subdirectory(src/graphics)
add_library(OpenGL_graphics)
add_subdirectory(src/graphics/OpenGL)
#add_library(geometry)
#add_subdirectory(src/geometry)

include_directories(src)

target_link_libraries(${PROJECT_NAME} PUBLIC
  ui
  #io 
  #geometry
  graphics
  OpenGL_graphics
  main_src)


#******************************************************************************************************************
# COPY ASSETS
#******************************************************************************************************************

add_custom_target(copy_assets
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets)

add_dependencies(${PROJECT_NAME} copy_assets)

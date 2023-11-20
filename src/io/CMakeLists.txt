target_sources(io
  PUBLIC  
    ${CMAKE_CURRENT_LIST_DIR}/reader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ioUtils.cpp
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/reader.h
    ${CMAKE_CURRENT_LIST_DIR}/ioUtils.h)

target_include_directories(io 
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_SOURCE_DIR}/dependencies/glm
    ${CMAKE_SOURCE_DIR}/dependencies/entt/include
    ${CMAKE_SOURCE_DIR}/src)
  #${CMAKE_SOURCE_DIR}/dependencies/glm
  #${CMAKE_SOURCE_DIR}/dependencies/glfw_3_3_8/include)
    #${CMAKE_SOURCE_DIR}/dependencies/msdf-atlas-gen/msdfgen
    #${CMAKE_SOURCE_DIR}/dependencies/msdf-atlas-gen/msdf-atlas-gen)

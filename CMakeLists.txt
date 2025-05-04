cmake_minimum_required(VERSION 3.22)
project(AStar VERSION 1.0)

# Dependencies
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
  endif()
endif()

set(CMAKE_C_STANDARD 11) # Requires C11 standard

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SRC
    src/astar.c
    src/astar.h
    src/dynamicArray.c
    src/dynamicArray.h
    src/int2.c
    src/int2.h
    src/main.c
    src/priorityQueue.c
    src/priorityQueue.h
    src/raygui.h
)
source_group(src   FILES ${SRC})

add_executable(AStar)

set_target_properties(
    AStar PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AStar)

target_sources(AStar PRIVATE ${SRC} ${IMGUI})

target_link_libraries(${PROJECT_NAME} raylib)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

add_custom_command(TARGET ${PROJECT_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/resources
                "$<TARGET_FILE_DIR:AStar>/resources")
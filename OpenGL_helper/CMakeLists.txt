cmake_minimum_required(VERSION 3.16)

if(DEFINED ENV{SRC_NAME})
  # 1. Get the string from environment
  set(SRC_STRING "$ENV{SRC_NAME}")

  # 2. Convert spaces to semicolons to make it a CMake list
  string(REPLACE " " ";" SRC_LIST "${SRC_STRING}")

  # 3. Resolve paths relative to the source directory
  set(SRC_NAME "")
  foreach(FILE_PATH ${SRC_LIST})
      # Removes the './' prefix if present and makes it absolute
      get_filename_component(ABS_FILE "${FILE_PATH}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")
      list(APPEND SRC_NAME "${ABS_FILE}")
  endforeach()
else()
  set(SRC_NAME "default_file.cpp")
endif()

# Check the result
message(STATUS "Files to compile: ${SRC_NAME}")



project(excute)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(VENDOR_ROOT "/home/devh/Build_EmbSys/Engines/Vendors")
set(platform "x86_64_u20")

find_package(GLEW REQUIRED)
# Tell the linker to find the NVIDIA Management Library
find_library(NVML_LIB NAMES nvidia-ml)

# --- Add GLFW ---
add_subdirectory("${VENDOR_ROOT}/glfw" "${CMAKE_BINARY_DIR}/glfw_build")

add_executable(${PROJECT_NAME}
    ${SRC_NAME}
)

# --- Link ---
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
# --- OpenGL ---
if(UNIX)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
    OpenGL::GL
    glfw
    GLEW::GLEW 
    ${NVML_LIB})
endif()

# --- Include dirs (GLAD & GLFW headers) ---
target_include_directories(${PROJECT_NAME} PRIVATE
    "${VENDOR_ROOT}/glfw/include"
    "${VENDOR_ROOT}/x86_64_u20/glad/include"
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/vendor"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}/"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/res"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
    COMMENT "Copying resources to build directory..."
)
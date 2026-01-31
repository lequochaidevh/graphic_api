cmake_minimum_required(VERSION 3.16)

if(DEFINED ENV{SRC_NAME})
  set(SRC_NAME $ENV{SRC_NAME})
else()
  set(SRC_NAME "default SRC_NAME")
endif()

project(excute)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VENDOR_ROOT "/home/devh/Build_EmbSys/Engines/Vendors")
set(platform "x86_64_u20")

find_package(GLEW REQUIRED)

# --- Add GLFW ---
add_subdirectory("${VENDOR_ROOT}/glfw" "${CMAKE_BINARY_DIR}/glfw_build")

# --- Add GLAD ---
# add_library(glad STATIC
#     "${VENDOR_ROOT}/x86_64_u20/glad/src/gl.c"
# )

# target_include_directories(glad PUBLIC
#     $<BUILD_INTERFACE:${VENDOR_ROOT}/x86_64_u20/include>
#     $<BUILD_INTERFACE:${VENDOR_ROOT}/x86_64_u20/include/glad>
#     $<INSTALL_INTERFACE:include/glad>
#     $<INSTALL_INTERFACE:include/KHR>
# )

add_executable(${PROJECT_NAME}
    ${SRC_NAME}
)

# --- Link ---
# target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
# --- OpenGL ---
if(UNIX)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
    OpenGL::GL
    glfw
    GLEW::GLEW )
endif()

# --- Include dirs (GLAD & GLFW headers) ---
target_include_directories(${PROJECT_NAME} PRIVATE
    "${VENDOR_ROOT}/glfw/include"
    "${VENDOR_ROOT}/x86_64_u20/glad/include"
)

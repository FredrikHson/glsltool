cmake_minimum_required(VERSION 3.7)

project(glsltool)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(ASSIMP REQUIRED assimp)
find_package(DevIL REQUIRED)

set(COMMON_SRC
    "main.c"
    "matrix.c"
    "notify.c"
    "options.c"
    "renderfunc.c"
    "resources.c"
    "resources/image.c"
    "resources/mesh.c"
    "resources/shader.c"
    "script.c"
    "script/input.c"
    "script/math.c"
    "script/render.c"
    "script/resources.c"
    "script/shader.c"
    "vector.c"
    )

include_directories(
    .
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${IL_INCLUDE_DIR}
    ../external/v7
    )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

add_library(v7 "../external/v7/v7.c")

add_executable(${CMAKE_PROJECT_NAME} ${COMMON_SRC})

target_link_libraries(${CMAKE_PROJECT_NAME}
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${IL_LIBRARIES}
    ${ILU_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    m
    v7
    )
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC "GL_GLEXT_PROTOTYPES")
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "-Werror")
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "-Wall")
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "-Wpedantic")
install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)

# uninstall target
configure_file(
    "${CMAKE_MODULE_PATH}/cmake_uninstall.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

find_program(iwyu_path NAMES include-what-you-use iwyu)
if(iwyu_path)
    set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY C_INCLUDE_WHAT_YOU_USE ${iwyu_path})
    set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()

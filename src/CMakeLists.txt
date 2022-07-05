cmake_minimum_required(VERSION 3.18)

project(glsltool)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenSSL REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(ASSIMP REQUIRED assimp)
find_package(DevIL REQUIRED)
set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

set(COMMON_SRC
    "audio.c"
    "debug.c"
    "generators.c"
    "generators/generic.c"
    "generators/plane.c"
    "jitterdata.c"
    "main.c"
    "matrix.c"
    "notify.c"
    "options.c"
    "renderfunc.c"
    "resources.c"
    "resources/image.c"
    "resources/mesh.c"
    "resources/shader.c"
    "resources/sound.c"
    "screenshot.c"
    "script.c"
    "script/audio.c"
    "script/file.c"
    "script/input.c"
    "script/math.c"
    "script/options.c"
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
    ../external/miniaudio/extras/miniaudio_split
    )

set(SHADERS
    "glsl/debug.frag"
    "glsl/debug_fp.frag"
    "glsl/debug.vert"
    )

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
foreach(SHADER ${SHADERS})
    string(REGEX REPLACE "[/\.]" "_" SHADER_NAME ${SHADER})
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.c
        COMMAND xxd -i  ${SHADER} > ${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.c
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        DEPENDS ${SHADER}
        )
    list(APPEND SHADER_DEPEND ${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.c)
endforeach()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

add_definitions(-DV7_ENABLE_FILE)
add_definitions(-DV7_BUILD_PROFILE=3)
add_definitions(-DV7_LARGE_AST)
add_library(v7 "../external/v7/v7.c")
target_compile_options(v7 PRIVATE "-O3")
target_compile_definitions(v7 PRIVATE "NDEBUG")
add_library(miniaudio "../external/miniaudio/extras/miniaudio_split/miniaudio.c")
target_compile_options(miniaudio PRIVATE "-O3")
target_compile_definitions(miniaudio PRIVATE "NDEBUG")


set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
add_executable(${CMAKE_PROJECT_NAME} ${COMMON_SRC} ${SHADER_DEPEND})

target_link_libraries(${CMAKE_PROJECT_NAME}
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${IL_LIBRARIES}
    ${ILU_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    ${OPENSSL_SSL_LIBRARIES}
    ${OPENSSL_CRYPTO_LIBRARY}
    miniaudio
    m
    dl
    Threads::Threads
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

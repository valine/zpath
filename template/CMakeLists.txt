cmake_minimum_required(VERSION 3.0)
set(THIS_PROJECT heartbeat)
project(${THIS_PROJECT})

file(GLOB_RECURSE sources src/core/*.cpp src/core/*.c src/core/*.h src/gui/*.cpp src/gui/*.c src/gui/*.h)
file(GLOB_RECURSE data resources/*)
include_directories(include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${THIS_PROJECT} ${sources} ${data})


target_compile_options(${THIS_PROJECT} PUBLIC -std=c++1y -Wall)
target_include_directories(${THIS_PROJECT} PUBLIC src/main)

file(COPY ${data} DESTINATION resources)

add_custom_command(TARGET ${THIS_PROJECT} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/resources)

add_custom_command(TARGET ${THIS_PROJECT} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/zpath/src/zpath/resources $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/resources)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

#
#   zpath
#
set(zpath_prefix      "${CMAKE_CURRENT_SOURCE_DIR}/deps/zpath")
set(zpath_install_dir "${zpath_prefix}")
set(zpath_cmake_args  "-DBUILD_LIB=ON;-DCMAKE_INSTALL_PREFIX=${zpath_install_dir}")
externalproject_add(zpath
                    PREFIX         ${zpath_prefix}
                    GIT_REPOSITORY https://github.com/valine/zpath.git
                    UPDATE_COMMAND ""
                    INSTALL_DIR    ${zpath_install_dir}
                    CMAKE_ARGS     ${zpath_cmake_args}
                    GIT_TAG develop
                    )

add_dependencies(${THIS_PROJECT} zpath)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/zpath/src/zpath/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/zpath/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/zpath/src/zpath/deps/zdep/src/zdep/headers/assimp/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}deps/zpath/src/zpath/deps/zdep/src/zdep/linux/assimp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/zpath/src/zpath/deps/zdep/src/zdep/headers/glfw/include)


link_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/zpath/src/zpath/deps/glfw/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/zpath/src/zpath/deps/zdep/src/zdep/headers/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/zpath/src/zpath/deps/zdep/src/zdep/headers/freetype/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/zpath/src/zpath/deps/png/include)

set(OpenGL_GL_PREFERENCE GLVND)
find_package (OpenGL REQUIRED)
include_directories(zpath ${OPENGL_INCLUDE_DIR})
link_libraries(zpath ${OPENGL_gl_LIBRARY})
find_package (Threads)

find_package(X11 REQUIRED)
message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")
include_directories(${X11_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR})

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
    message(STATUS "Linux detected")
endif()

set(ZDEP "${CMAKE_CURRENT_SOURCE_DIR}/deps/zpath/src/zpath/")

if (APPLE)
    if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(zdep_path    "${ZDEP}/deps/zdep/src/zdep/macos")
        set(zdep_path_include    "${ZDEP}/deps/zdep/src/zdep/headers")
    else()
        set(zdep_path    "${ZDEP}/deps/zdep/src/zdep/macos-arm")
        set(zdep_path_include    "${ZDEP}/deps/zdep/src/zdep/headers")
    endif()
else()
    if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(zdep_path    "${ZDEP}/deps/zdep/src/zdep/linux")
        set(zdep_path_include    "${ZDEP}/deps/zdep/src/zdep/headers")
    else()
        set(zdep_path    "${ZDEP}/deps/zdep/src/zdep/macos-arm")
        set(zdep_path_include    "${ZDEP}/deps/zdep/src/zdep/headers")
    endif()
endif()

target_link_libraries (${THIS_PROJECT}
        ${OPENGL_LIBRARIES}
        ${zpath_install_dir}/lib/libzpath.a
        "${zdep_path}/glfw/libglfw3.a"
        ${X11_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${CMAKE_DL_LIBS}
        "${zdep_path}/giac/libgiac.a"
        "${zdep_path}/assimp/libassimp.so"
        "${zdep_path}/png/libpng16.a"
        "${zdep_path}/zlib/libz.a"
        "${zdep_path}/freetype/libfreetype.a"
        "${zdep_path}/gmp/libgmp.a"
        "${zdep_path}/gsl/libgsl.a"
        "${zdep_path}/mpfr/libmpfr.a"
        "${zdep_path}/ntl/libntl.a"
        "${zdep_path}/mpfi/libmpfi.a"
        "${zdep_path}/pari/libpari.a")



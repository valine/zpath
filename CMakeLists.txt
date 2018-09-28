cmake_minimum_required(VERSION 3.0) # setting this is required
project(zpath)            # this sets the project name

option(BUILD_LIB "help string describing option" OFF)

###############################################################################
## file globbing ##############################################################
###############################################################################

file(GLOB_RECURSE sources      src/main/*.cpp src/main/*.vs src/main/*.fs)
file(GLOB_RECURSE sources_test src/test/*.cpp)
file(GLOB_RECURSE data resources/*)

include_directories(include)

###############################################################################
## target definitions #########################################################
###############################################################################

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (BUILD_LIB) 
  add_library(zpath STATIC ${sources} ${headers})
  target_compile_options(zpath PUBLIC -std=c++1y -Wall)
   target_include_directories(zpath PUBLIC src/main)
else(BUILD_LIB)
  add_executable(zpath ${sources} ${data})
  target_compile_options(zpath PUBLIC -std=c++1y -Wall)
  file(COPY ${data} DESTINATION resources)
  add_custom_command(TARGET zpath PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:zpath>/resources)
  target_include_directories(zpath PUBLIC src/main)
endif()


###############################################################################
## dependencies ###############################################################
###############################################################################

#glfw
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
#
#   GLFW
#
set(glfw_prefix      "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw")
set(glfw_install_dir "${glfw_prefix}")
set(glfw_cmake_args  "-DCMAKE_INSTALL_PREFIX=${glfw_install_dir}")
externalproject_add(glfw
                    PREFIX         ${glfw_prefix}
                    GIT_REPOSITORY https://github.com/glfw/glfw.git
                    UPDATE_COMMAND ""
                    INSTALL_DIR    ${glfw_install_dir}
                    CMAKE_ARGS     ${glfw_cmake_args}
                    LOG_DOWNLOAD ON
                    )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/lib)
add_dependencies(zpath glfw)

#
#   assimp
#
set(assimp_prefix      "${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp")
set(assimp_install_dir "${assimp_prefix}")
set(assimp_cmake_args  "-DCMAKE_INSTALL_PREFIX=${assimp_install_dir}")
externalproject_add(assimp
                    PREFIX         ${assimp_prefix}
                    GIT_REPOSITORY https://github.com/assimp/assimp.git
                    UPDATE_COMMAND ""
                    GIT_TAG        v4.1.0
                    INSTALL_DIR    ${assimp_install_dir}
                    CMAKE_ARGS     ${assimp_cmake_args}
                    LOG_DOWNLOAD ON
                    )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp/lib)
add_dependencies(zpath assimp)

#
#   glm
#
set(glm_prefix      "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm")
set(glm_install_dir "${glm_prefix}")
set(glm_cmake_args  "-DCMAKE_INSTALL_PREFIX=${glm_install_dir}")
externalproject_add(glm
        PREFIX  ${glm_prefix}
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        0.9.9.0 
        UPDATE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
    )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/src/glm/)
add_dependencies(zpath glm)

  # #
  # #   freetype
  # #
  # set(freetype_prefix "${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype")
  # set(freetype_install_dir "${freetype_prefix}")
  # set(freetype_cmake_args  "-DCMAKE_INSTALL_PREFIX=${freetype_install_dir}")
  # externalproject_add(freetype
  #       PREFIX  ${freetype_prefix}
  #       GIT_REPOSITORY git@github.com:ubawurinna/freetype-windows-binaries.git
  #       UPDATE_COMMAND ""
  #       BUILD_COMMAND ""
  #       INSTALL_DIR ""
  #       CMAKE_ARGS ""
  #       INSTALL_COMMAND cmake -E echo "Skipping install step."
  #       BUILD_COMMAND cmake -E echo "Skipping build step."
  #       LOG_DOWNLOAD ON
  #   )

  # # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/src/glm/)
  # add_dependencies(zpath freetype)

#Opengl
find_package (OpenGL REQUIRED)
include_directories(zpath ${OPENGL_INCLUDE_DIR})
link_libraries(zpath ${OPENGL_gl_LIBRARY})
find_package (Threads)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (APPLE)
  find_package(glfw3 3.2 REQUIRED)
  include_directories(/usr/local/include)
  find_library(COCOA_LIBRARY Cocoa REQUIRED)
  find_library(IOKIT_LIBRARY IOKit REQUIRED)
  find_library(COREVID_LIBRARY CoreVideo REQUIRED)
  find_library(fr_coretext CoreText)
  find_library(fr_corefoundation CoreFoundation)
  find_library(fr_coregraphics CoreGraphics)

  target_link_libraries (zpath
          glfw ${GLFW_LIBRARIES}
          ${OPENGL_LIBRARIES}
          ${COCOA_LIBRARY} ${COREVID_LIBRARY} ${IOKIT_LIBRARY}
          ${fr_coretext}
          ${fr_corefoundation}
          ${fr_coregraphics}
          ${FREETYPE_LIBRARIES})

endif (APPLE)

if (LINUX)
  find_package(Freetype REQUIRED)
  include_directories(${FREETYPE_INCLUDE_DIRS})
  find_package(X11 REQUIRED)
  message(STATUS "X11_FOUND = ${X11_FOUND}")
  message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
  message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")
  include_directories(${X11_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR})
  target_link_libraries (zpath
          ${OPENGL_LIBRARIES}
          "${glfw_install_dir}/lib/libglfw3.a"
          ${X11_LIBRARIES}
          ${CMAKE_THREAD_LIBS_INIT}
          ${CMAKE_DL_LIBS}
          ${FREETYPE_LIBRARIES}
          "${assimp_install_dir}/lib/libassimp.so")

endif (LINUX)

if (WIN32)
      #Opengl
  find_package (OpenGL REQUIRED)
  include_directories(zpath ${OPENGL_INCLUDE_DIR})
  link_libraries(zpath ${OPENGL_gl_LIBRARY})
  find_package (Threads)


  find_package(X11 REQUIRED)
  message(STATUS "X11_FOUND = ${X11_FOUND}")
  message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
  message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")
  include_directories(${X11_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR})
  include_directories("C:/Users/Lukas/Desktop/freetype-windows-binaries/include")
  target_link_libraries (zpath
          ${OPENGL_LIBRARIES}
          "${glfw_install_dir}/lib/glfw3.lib"
          ${X11_LIBRARIES}
          ${CMAKE_THREAD_LIBS_INIT}
          ${CMAKE_DL_LIBS}
          "${assimp_install_dir}/lib/assimp-vc140-mt.lib"
          "C:/Users/Lukas/Desktop/freetype-windows-binaries/win32/freetype.lib")

endif(WIN32)

###############################################################################
## packaging ##################################################################
###############################################################################

install (TARGETS zpath
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin)



SET(CPACK_OUTPUT_FILE_PREFIX packages)
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

set(CPACK_PACKAGE_NAME "Zpath")
set(CPACK_PACKAGE_VERSION "0.3")
set(CPACK_MONOLITHIC_INSTALL 1)

include(CPack)
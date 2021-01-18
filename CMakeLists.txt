cmake_minimum_required(VERSION 3.0) # setting this is required
project(zpath)            # this sets the project name

option(BUILD_LIB "help string describing option" OFF)

option(COMPILE_DEPS "help string describing option" OFF)

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
  target_compile_options(zpath PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++14>)
   target_include_directories(zpath PUBLIC src/main)
else(BUILD_LIB)
  add_executable(zpath ${sources} ${data} src/main/utils/zfontstore.cpp include/utils/zfontstore.h src/main/ui/viewGroup/ztabview.cpp include/ui/ztabview.h src/main/ui/viewController/ztabbedviewcontroller.cpp include/ztabbedviewcontroller.h src/main/zpath/nodeviewcontroller.cpp include/zpath/nodeviewcontroller.h src/main/ui/nodeEditor/zlineview.cpp include/ui/zlineview.h src/main/ui/nodeEditor/znodeeditor.cpp include/ui/znodeeditor.h src/main/ui/nodeEditor/nodeview.cpp include/ui/nodeview.h src/main/ui/nodeEditor/zmagnitudepicker.cpp include/ui/zmagnitudepicker.h src/main/ui/chart/zlinechart.cpp include/ui/zlinechart.h src/main/ui/nodeEditor/zcursorview.cpp include/ui/zcursorview.h src/main/utils/casutil.cpp include/utils/casutil.h src/main/utils/znodeutil.cpp include/utils/znodeutil.h)
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
set(OpenGL_GL_PREFERENCE GLVND)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

if (COMPILE_DEPS)

  if (WIN32)
  set(freetype_prefix      "${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype")
  set(freetype_install_dir "${freetype_prefix}")
  set(freetype_cmake_args  "")
  externalproject_add(freetype
                      PREFIX         ${freetype_prefix}
                      GIT_REPOSITORY https://github.com/ubawurinna/freetype-windows-binaries.git
                      CMAKE_ARGS     ${freetype_cmake_args}
                      UPDATE_COMMAND ""
                      CONFIGURE_COMMAND ""
                      BUILD_COMMAND ""
                      INSTALL_COMMAND ""
                      LOG_DOWNLOAD ON
                      )
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype/src/freetype/include)
  add_dependencies(zpath freetype)

  endif(WIN32)


  #glfw
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
                      GIT_SHALLOW 1
                      INSTALL_DIR    ${glfw_install_dir}
                      CMAKE_ARGS     ${glfw_cmake_args}
                      LOG_DOWNLOAD ON
                      )
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include)
  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/lib)
  add_dependencies(zpath glfw)


  #libpng
  #
  #   png
  #

  set(zlib_prefix      "${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib")
  set(zlib_install_dir "${zlib_prefix}")
  set(zlib_cmake_args  "-DCMAKE_INSTALL_PREFIX=${zlib_install_dir}")
  externalproject_add(zlib
          PREFIX         ${zlib_prefix}
          GIT_REPOSITORY https://github.com/madler/zlib.git
          UPDATE_COMMAND ""
          GIT_SHALLOW 1
          INSTALL_DIR    ${zlib_install_dir}
          CMAKE_ARGS     ${zlib_cmake_args}
          LOG_DOWNLOAD ON
          )
  add_dependencies(zpath zlib)

  set(png_prefix      "${CMAKE_CURRENT_SOURCE_DIR}/deps/png")
  set(png_install_dir "${png_prefix}")
  set(png_cmake_args  "-DCMAKE_INSTALL_PREFIX=${png_install_dir}")
  externalproject_add(png
          PREFIX         ${png_prefix}
          GIT_REPOSITORY git://git.code.sf.net/p/libpng/code
          UPDATE_COMMAND ""
          GIT_SHALLOW 1
          INSTALL_DIR    ${png_install_dir}
          CMAKE_ARGS     ${png_cmake_args}
          LOG_DOWNLOAD ON
          )

  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/png/src/png-build)
  add_dependencies(zpath png)

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
                      GIT_SHALLOW 1
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
          GIT_SHALLOW 1
          BUILD_COMMAND ""
          INSTALL_COMMAND ""
          LOG_DOWNLOAD ON)

  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/src/glm/)
  add_dependencies(zpath glm)


  # #
  # #   freetype
  # #
  set(freetype2b_prefix      "${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype2b")
  set(freetype2b_install_dir "${freetype2b_prefix}")
  ExternalProject_Add(
          freetype2b
          PREFIX  ${freetype2b_prefix}
          GIT_REPOSITORY git://git.savannah.gnu.org/freetype/freetype2.git
          GIT_SHALLOW 1
          CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${freetype2b_prefix} -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DWITH_PNG=Off
          UPDATE_COMMAND ""
          INSTALL_COMMAND "")

  add_dependencies(zpath freetype2b)


else(COMPILE_DEPS)

  set(zdep_prefix      "${CMAKE_CURRENT_SOURCE_DIR}/deps/zdep")
  set(zdep_install_dir "${zdep_prefix}")
  set(zdep_cmake_args  "-DCMAKE_INSTALL_PREFIX=${zlib_install_dir}")
  externalproject_add(zdep
          PREFIX         ${zdep_prefix}
          GIT_REPOSITORY https://github.com/valine/zpath-deps.git
          GIT_SHALLOW 1
          BUILD_COMMAND ""
          INSTALL_COMMAND ""
          LOG_DOWNLOAD ON)
  add_dependencies(zpath zdep)

endif(COMPILE_DEPS)

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

  find_package(X11 REQUIRED)
  message(STATUS "X11_FOUND = ${X11_FOUND}")
  message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
  message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")
  include_directories(${X11_INCLUDE_DIR})

  if (COMPILE_DEPS)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype2b/src/freetype2b/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/png/include)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib/lib)
    include_directories(${ASSIMP_INCLUDE_DIR})
    target_link_libraries (zpath
            ${OPENGL_LIBRARIES}
            "${glfw_install_dir}/lib/libglfw3.a"
            ${X11_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT}
            ${CMAKE_DL_LIBS}
            "${assimp_install_dir}/lib/libassimp.so"
            "${CMAKE_CURRENT_SOURCE_DIR}/deps/png/lib/libpng.a"
            "${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib/lib/libz.a"
            "${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype2b/src/freetype2b-build/libfreetype.a")

  else(COMPILE_DEPS)
    set(zdep_path    "${CMAKE_CURRENT_SOURCE_DIR}/deps/zdep/src/zdep/linux")
    message(STATUS "${zdep_path}")

    include_directories(${zdep_path}/glm)
    include_directories(${zdep_path}/glfw/include)
    include_directories(${zdep_path}/freetype/include)
    include_directories(${zdep_path}/zlib/include)
    include_directories(${zdep_path}/assimp/include)
    include_directories(${zdep_path}/assimp/include)
    include_directories(${zdep_path}/giac/include)
    target_link_libraries (zpath
            ${OPENGL_LIBRARIES}
            "${zdep_path}/glfw/libglfw3.a"
            ${X11_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT}
            ${CMAKE_DL_LIBS}
            "${zdep_path}/assimp/libassimp.so"
            "${zdep_path}/png/libpng16.a"
            "${zdep_path}/zlib/libz.a"
            "${zdep_path}/freetype/libfreetype.a"
            "${zdep_path}/giac/libgiac.so")
  endif(COMPILE_DEPS)

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
 
  target_link_libraries (zpath
          ${OPENGL_LIBRARIES}
          "${glfw_install_dir}/lib/glfw3.lib"
          ${X11_LIBRARIES}
          ${CMAKE_THREAD_LIBS_INIT}
          ${CMAKE_DL_LIBS}
          ${CMAKE_CURRENT_SOURCE_DIR}/deps/png/src/png-build/png.lib
          "${assimp_install_dir}/lib/assimp-vc140-mt.lib"
          ${freetype_install_dir}/src/freetype/win32/freetype.lib)

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

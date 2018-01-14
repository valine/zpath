cmake_minimum_required(VERSION 3.0) # setting this is required
project(zpath)            # this sets the project name

###############################################################################
## file globbing ##############################################################
###############################################################################

file(GLOB_RECURSE sources      src/main/*.cpp src/main/*.h)
file(GLOB_RECURSE sources_test src/test/*.cpp)
file(GLOB_RECURSE data resources/*)

###############################################################################
## target definitions #########################################################
###############################################################################

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(zpath ${sources} ${data})

target_compile_options(zpath PUBLIC -std=c++1y -Wall)

target_include_directories(zpath PUBLIC src/main)

file(COPY ${data} DESTINATION resources)

add_custom_command(TARGET zpath PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:zpath>/resources)

###############################################################################
## dependencies ###############################################################
###############################################################################

#Opengl
find_package (OpenGL REQUIRED)
include_directories(zpath ${OPENGL_INCLUDE_DIR})
link_libraries(zpath ${OPENGL_gl_LIBRARY})

find_package (Threads)

if (WIN32)

  include_directories("C:/glew/include")
  link_directories("C:/glew/lib/release/Win32")


  include_directories("C:/Program Files (x86)/glfw3/lib")
  link_directories("C:/Program Files (x86)/glfw3/include/GLFW")


  include_directories("C:/glm")
else (WIN32)
  find_package (GLEW REQUIRED STATIC)

    #glm
  find_package (glm REQUIRED)
  link_libraries(zpath ${GLM_LIBRARY})

  # glfw
find_package(glfw3 3.2 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
endif (WIN32)

if (APPLE)
  include_directories(/usr/local/include)
  find_library(COCOA_LIBRARY Cocoa REQUIRED)
  find_library(IOKIT_LIBRARY IOKit REQUIRED)
  find_library(COREVID_LIBRARY CoreVideo REQUIRED)

  target_link_libraries (zpath
          glfw3
          ${OPENGL_LIBRARIES}
          ${GLEW_LIBRARY}
          ${COCOA_LIBRARY} ${COREVID_LIBRARY} ${IOKIT_LIBRARY})

endif (APPLE)

if (WIN32)
  target_link_libraries (
    zpath
    ${OPENGL_LIBRARIES} 
    gdi32
    glfw3
    glew32s 
    opengl32)
endif (WIN32)

if (UNIX)

  find_package(X11 REQUIRED)

  message(STATUS "X11_FOUND = ${X11_FOUND}")
  message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
  message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

  include_directories(${X11_INCLUDE_DIR})

  target_link_libraries (zpath
          ${OPENGL_LIBRARIES}
          ${GLEW_LIBRARY}
          glfw3
          ${X11_LIBRARIES}
          ${CMAKE_THREAD_LIBS_INIT}
          ${CMAKE_DL_LIBS})

endif (UNIX)

###############################################################################
## testing ####################################################################
###############################################################################

find_package(GTest)

if(GTEST_FOUND)
  add_executable(unit_tests ${sources_test} ${sources})

  target_compile_definitions(unit_tests PUBLIC UNIT_TESTS)

  set_target_properties(zpath PROPERTIES ENABLE_EXPORTS on)

  target_link_libraries(unit_tests PUBLIC
    ${GTEST_BOTH_LIBRARIES}
    zpath
  )

  target_include_directories(unit_tests PUBLIC
    ${GTEST_INCLUDE_DIRS} 
  )
  
endif()

###############################################################################
## packaging ##################################################################
###############################################################################

SET(CPACK_OUTPUT_FILE_PREFIX packages)

set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

install(TARGETS zpath DESTINATION ".")

install(DIRECTORY resources DESTINATION ".")

if (WIN32)
  install(FILES "C:/zpath-libs/MSVCP140D.dll" DESTINATION .)
  install(FILES "C:/zpath-libs/ucrtbased.dll" DESTINATION .)
  install(FILES "C:/zpath-libs/VCRUNTIME140D.dll" DESTINATION .)
endif (WIN32)

set(CPACK_PACKAGE_NAME "Zpath")
set(CPACK_PACKAGE_VERSION "0.1.1")

set(CPACK_MONOLITHIC_INSTALL 1)

include(CPack)
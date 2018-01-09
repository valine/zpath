cmake_minimum_required(VERSION 3.0) # setting this is required
project(zpath)            # this sets the project name

###############################################################################
## file globbing ##############################################################
###############################################################################

# These instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables 
# `sources` and `data`.
file(GLOB_RECURSE sources      src/main/*.cpp src/main/*.h)
file(GLOB_RECURSE sources_test src/test/*.cpp)
file(GLOB_RECURSE data resources/*)
# You can use set(sources src/main.cpp) etc if you don't want to
# use globbing to find files automatically.

###############################################################################
## target definitions #########################################################
###############################################################################

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# The data is just added to the executable, because in some IDEs (QtCreator) 
# files are invisible when they are not explicitly part of the project.
add_executable(zpath ${sources} ${data})


# Just for example add some compiler flags.
target_compile_options(zpath PUBLIC -std=c++1y -Wall)

# This allows to include files relative to the root of the src directory with a <> pair
target_include_directories(zpath PUBLIC src/main)


# This copies all resource files in the build directory.
# We need this, because we want to work with paths relative to the executable.
file(COPY ${data} DESTINATION resources)

###############################################################################
## dependencies ###############################################################
###############################################################################

# This defines the variables Boost_LIBRARIES that containts all library names
# that we need to link into the program.
#find_package(Boost 1.36.0 COMPONENTS filesystem system REQUIRED)

# # glfw
# find_package(glfw3 REQUIRED)
# include_directories(${GLFW_INCLUDE_DIRS})
# link_libraries(${GLFW_LIBRARY_DIRS})

# # opengl
# find_package(OpenGL REQUIRED)
# include_directories(${OPENGL_INCLUDE_DIRS})

# # glew
# if (WIN32)
#   list(APPEND CMAKE_PREFIX_PATH "C:/Program Files (x86)/GLEW")
# endif (WIN32)

# find_package (GLEW REQUIRED STATIC)
# if (GLEW_FOUND)
#     set(GLEW_USE_STATIC_BUILD ON)
#     include_directories(${GLEW_INCLUDE_DIRS})
#     link_directories("C:/Program Files (x86)/GLEW/lib")
#     link_libraries(${GLEW_LIBRARIES})
# endif()

# target_link_libraries(
#         zpath
#         glfw
#         ${OPENGL_LIBRARIES}
#         ${GLEW_LIBRARIES}
#         ${EXTRA_LIBS}
#   )



find_package (OpenGL REQUIRED)

# glfw
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
link_libraries(${GLFW_LIBRARY_DIRS})

if (WIN32)
  include_directories("C:/glew/include")
  link_directories("C:/glew/lib/release/Win32")
else (WIN32)
  find_package (GLEW REQUIRED STATIC)
endif (WIN32)
if (APPLE)
  include_directories(/usr/local/include)
  find_library(COCOA_LIBRARY Cocoa REQUIRED)
  find_library(IOKIT_LIBRARY IOKit REQUIRED)
  find_library(COREVID_LIBRARY CoreVideo REQUIRED)
endif (APPLE)

#add_executable(zpath ${SOURCE_FILES})
  target_link_libraries (zpath
          glfw
          ${OPENGL_LIBRARIES}
          ${GLEW_LIBRARY}
          ${COCOA_LIBRARY} ${COREVID_LIBRARY} ${IOKIT_LIBRARY})

if (WIN32)
  target_link_libraries (zpath
          ${OPENGL_LIBRARIES} glfw glew32s opengl32)
endif (WIN32)

if (UNIX)
  target_link_libraries (zpath
          ${OPENGL_LIBRARIES}
          ${GLFW3_LIBRARY}
          ${GLEW_LIBRARY})
endif (UNIX)
#
#target_link_libraries(zpath PUBLIC
 # ${Boost_LIBRARIES}
  # here you can add any library dependencies
#)

###############################################################################
## testing ####################################################################
###############################################################################

# This is for our testing framework,
# we don't add REQUIRED because it's just for testing.
# People who might want to build the project to use it should not be required
# to install testing dependencies.
find_package(GTest)

if(GTEST_FOUND)
  add_executable(unit_tests ${sources_test} ${sources})

  # This define is added to prevent collision with the main.
  # It might be better solved by not adding the source with the main to the
  # testing target.
  target_compile_definitions(unit_tests PUBLIC UNIT_TESTS)

  # This allows us to use the executable as a link library, and inherit all 
  # linker options and library dependencies from it, by simply adding it as dependency.
  set_target_properties(zpath PROPERTIES ENABLE_EXPORTS on)

  target_link_libraries(unit_tests PUBLIC
    ${GTEST_BOTH_LIBRARIES}
    zpath
  )

  target_include_directories(unit_tests PUBLIC
    ${GTEST_INCLUDE_DIRS} # doesn't do anything on linux
  )
  
endif()

###############################################################################
## packaging ##################################################################
###############################################################################

SET(CPACK_OUTPUT_FILE_PREFIX packages)
#SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 1)

set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
# All install commands get the same destination. this allows us to use paths
# relative to the executable.
install(TARGETS zpath DESTINATION ".")

# This is basically a repeat of the file copy instruction that copies the
# resources in the build directory, but here we tell cmake that we want it
# in the package.
install(DIRECTORY resources DESTINATION ".")

# Now comes everything we need, to create a package
# there are a lot more variables you can set, and some
# you need to set for some package types, but we want to
# be minimal here.
set(CPACK_PACKAGE_NAME "Zpath")
set(CPACK_PACKAGE_VERSION "0.1.0")

# We don't want to split our program up into several incomplete pieces.
set(CPACK_MONOLITHIC_INSTALL 1)


# This must be last
include(CPack)
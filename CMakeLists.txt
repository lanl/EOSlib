PROJECT(EOSlib)

cmake_minimum_required( VERSION 3.0 )

set(projectName "EOSlib")
set(projectVersion "2.0.0")

# Enable unit testing with the Boost framework.
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
Include_directories(${Boost_INCLUDE_DIRS})

# verbose traversal is disabled by default. in general, a trace of the
# source tree traversal just adds noise to the configuration output and
# may obscure other more important messages indiciating errors or warnings
# however, a trace of the traversal can be very helpful when the
# configuration itself is broken.
#
# this option can be enable when invoking cmake by passing
# '-D verbose_traversal=TRUE'
if(NOT DEFINED verbose_traversal)
  set(verbose_traversal FALSE)
endif(NOT DEFINED verbose_traversal)

# by default, projects should be built for debugging. this is useful
# in a development environment where you might swearing at your terminal
# from time-to-time.
#
# the build type default can be overwritten when invoking cmake
# by passing a definition for the build type.
# '-D build_type={option}'
# where option is
# + {debug}    - using this option will provide debug symbols
# + {coverage} - as debug, but also provide flags for use with profilers
# + {release}  - using this option will provide optimizations suitable
#                for the platform architecture
# + {native}   - as release, but also provide optimizations specific to
#                the processor
#
# in addition, the default build type (user-specified or otherwise) may
# be overriden for a specific projects by passing a definition as part
# of the cmake invocation
# '-D {projectName}_build_type={option}
if(NOT DEFINED build_type)
  set( build_type "debug")
endif(NOT DEFINED build_type)

if(NOT DEFINED ${projectName}_build_type)
  set( ${projectName}_build_type "${build_type}" )
endif(NOT DEFINED ${projectName}_build_type)

set(local_testing TRUE)
set(build_tree "${CMAKE_BINARY_DIR}")

# by default, libraries are generated as dynamic libraries which are
# linked at run time. This reduces the linking step overhead during
# recompilation following a small change to the source code, which is
# helpful during TDD iteration.
#
# The default libary type may be specified to be statically linked
# during cmake invocation by passing
# '-D static_libraries=TRUE'
#
# in addition, the default library type (user-specified or otherwise) may
# be overriden for a specific projects by passing a definition as part
# of the cmake invocation
# '-D {projectName}_static_libraries=TRUE/FALSE
if(NOT DEFINED static_libraries)
  set(static_libraries FALSE)
endif(NOT DEFINED static_libraries)

if(NOT DEFINED ${projectName}_static_libraries)
  set(${projectName}_static_libraries ${static_libraries})
endif(NOT DEFINED ${projectName}_static_libraries)

if(${${projectName}_static_libraries})
  set(${projectName}_policy STATIC)
else(${${projectName}_static_libraries})
  set(${projectName}_policy SHARED)
endif(${${projectName}_static_libraries})

# normally, it's best to allow the configuration script setup 
# compilation flags derived from other user-specified defintions.
#
# That said, means to augment or over-write the compiler flags are
# provided, either in general or for a specific project 
# 
if(DEFINED compiler_flags)
  if (NOT DEFINED ${projectName}_compiler_flags)
    set(${projectName}_compiler_flags "${compiler_flags}")
  endif (NOT DEFINED ${projectName}_compiler_flags)
endif(DEFINED compiler_flags)

if (NOT DEFINED ${projectName}_appended_flags)
  set(${projectName}_appended_flags "${appended_flags}" )
endif (NOT DEFINED ${projectName}_appended_flags)

# if this is the first project directory traversed and the build
# tree is clean, it will be necessary to create the header directory
# shared amongst components 
if(NOT(EXISTS "${build_tree}/include"))
  file(MAKE_DIRECTORY "${build_tree}/include")
endif(NOT(EXISTS "${build_tree}/include"))

set(HEADER_DIR "${build_tree}/include")

# --------------------
# Begin project scopes
# --------------------
if(NOT DEFINED ${projectName}_header_source_directory)
  set(${projectName}_header_source_directory ${CMAKE_CURRENT_SOURCE_DIR})
endif(NOT DEFINED ${projectName}_header_source_directory)

if(NOT DEFINED ${projectName}_src_header)
  set(${projectName}_src_header ${projectName}.hpp.in)
endif(NOT DEFINED ${projectName}_src_header)

if(NOT DEFINED ${projectName}_bin_header)
  set(${projectName}_bin_header ${projectName}.hpp)
endif(NOT DEFINED ${projectName}_bin_header)

project (${projectName} VERSION ${projectVersion} LANGUAGES CXX)
enable_testing()

# message to indicate the order of traversal,
# which is often helpful for debugging
if(${verbose_traversal})
  message(STATUS "Entered ${projectName} directory")
endif(${verbose_traversal})

# Set up paths for the flattened header tree
if(NOT(EXISTS "${HEADER_DIR}/${projectName}"))
  file(MAKE_DIRECTORY "${HEADER_DIR}/${projectName}")

endif(NOT(EXISTS "${HEADER_DIR}/${projectName}"))

set(${projectName}_HEADER_DIR "${HEADER_DIR}/${projectName}")

# Initialize a variable for collecting utility implementation components
list(APPEND ${projectName}_src) # (empty list)
list(APPEND ${projectName}_src_headers) # (empty list)
list(APPEND ${projectName}_bin_headers) # (empty list)

# setup test labels
list(APPEND testLabels ${projectName})

# Gather and set revision information
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE ${projectName}_GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE ${projectName}_GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

# setup library type
if(NOT ${projectName}_static_libraries)
  set(BUILD_SHARED_LIBS TRUE)
else(NOT ${projectName}_static_libraries)
  set(BUILD_SHARED_LIBS FALSE)
endif(NOT ${projectName}_static_libraries)

# setup compiler flags
if(DEFINED ${projectName}_compiler_flags)
  set( CMAKE_CXX_FLAGS
    "${${projectName}_compiler_flags} ${${projectName}_appended_flags}")
  set(${projectName}_flags "${CMAKE_CXX_FLAGS}")
  
else(DEFINED ${projectName}_compiler_flags)
  if( ${${projectName}_build_type} STREQUAL "debug" )
    set( CMAKE_BUILD_TYPE DEBUG )
  elseif( ${${projectName}_build_type} STREQUAL "coverage" )
    set( CMAKE_BUILD_TYPE DEBUG )
  elseif( ${${projectName}_build_type} STREQUAL "release" )
    set( CMAKE_BUILD_TYPE RELEASE )
  elseif( ${${projectName}_build_type} STREQUAL "native" )
    set( CMAKE_BUILD_TYPE RELEASE )
  endif( ${${projectName}_build_type} STREQUAL "debug" )

  # base flags used by every project
  set( ${projectName}_flags
    "-std=c++11 -Wall -Wextra ${CMAKE_CXX_FLAGS} -Wno-c++11-compat-deprecated-writable-strings -Wno-unused-local-typedef -Wno-writable-strings -Wno-unused-parameter" )

  # debug variants
  if (${CMAKE_BUILD_TYPE} STREQUAL DEBUG)
    set( ${projectName}_flags
      "-gdwarf-3 ${CMAKE_CXX_FLAGS_DEBUG} ${${projectName}_flags}" )
    if( ${${projectName}_build_type} STREQUAL "coverage" )
      set( ${projectName}_flags
        "-fprofile-arcs -ftest-coverage ${${projectName}_flags}" )
    endif( ${${projectName}_build_type} STREQUAL "coverage" )
  endif (${CMAKE_BUILD_TYPE} STREQUAL DEBUG)
  
  # optimized variants
  if (${CMAKE_BUILD_TYPE} STREQUAL RELEASE)
    set( ${projectName}_flags
      "-flto ${CMAKE_CXX_FLAGS_RELEASE} ${${projectName}_flags}")
    if( ${${projectName}_build_type} STREQUAL "native" )
      set( ${projectName}_flags
        "-march=native ${${projectName}_flags}")
    endif( ${${projectName}_build_type} STREQUAL "native" )
  endif (${CMAKE_BUILD_TYPE} STREQUAL RELEASE)
  
  set(${projectName}_flags
    "${${projectName}_flags} ${${projectName}_appended_flags}")
  
  set( CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}
    ${${projectName}_flags})
  
endif(DEFINED ${projectName}_compiler_flags)

# A short function for CTest setup used in subdirectories
function(setupTest TEST_NAME)
  if(${verbose_traversal})
    message(STATUS "Entered test subdirectory")
  endif(${verbose_traversal})
  set(EXE_NAME ${TEST_NAME}.test)
  file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.C")
  add_executable(${EXE_NAME} ${SRC} )
  target_link_libraries( ${EXE_NAME} LINK_PUBLIC ${Boost_LIBRARIES} ${TEST_NAME})
  add_test( ${TEST_NAME} ${EXE_NAME} "--log_level=message")
  set_property( TEST ${TEST_NAME} PROPERTY PROCESSORS 1 )
  set_property( TEST ${TEST_NAME} APPEND PROPERTY LABELS ${testLabels} )
  if(${verbose_traversal})
    message(STATUS "Exiting test subdirectory")
  endif(${verbose_traversal})
endfunction(setupTest)

# setup include directory
include_directories( "${HEADER_DIR}" )
include_directories( "${CMAKE_CURRENT_BINARY_DIR}" )

message(STATUS "")
message(STATUS "-----------------------------------------------------------")
message(STATUS "")
message(STATUS "${projectName} Version: ${${projectName}_VERSION}")
message(STATUS "Git current branch: ${${projectName}_GIT_BRANCH}")
message(STATUS "Git commit hash: ${${projectName}_GIT_HASH}")
message(STATUS "")
message(STATUS "${projectName} path: ${PROJECT_SOURCE_DIR}")
message(STATUS "")
message(STATUS "${projectName} flags: ${${projectName}_flags}")
message(STATUS "")
message(STATUS "-----------------------------------------------------------")
message(STATUS "")

if(NOT DEFINED EOSLIB_DATA_TEST_PATH)
  set(EOSLIB_DATA_TEST_PATH "${CMAKE_CURRENT_BINARY_DIR}/DATA")
endif(NOT DEFINED EOSLIB_DATA_TEST_PATH)

if(NOT DEFINED EOSLIB_DATA_PATH)
  set(EOSLIB_DATA_PATH "${EOSLIB_DATA_TEST_PATH}")
endif(NOT DEFINED EOSLIB_DATA_PATH)

set(CTEST_ENVIRONMENT
  "EOSLIB_DATA_PATH=${EOSLIB_DATA_TEST_PATH}"
  "CTEST_OUTPUT_ON_FAILURE=1"
  "BOOST_TEST_LOG_LEVEL='message'"
  )

add_subdirectory(src)
add_subdirectory(DATA)

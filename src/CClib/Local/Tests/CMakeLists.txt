if(${verbose_traversal})
  message(STATUS "Entered Local subdirectory")
endif(${verbose_traversal})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
#
# simplistic tests pass that only check exit status
#
file(GLOB Test_c  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.C")

foreach( test_c ${Test_c} )
  get_filename_component( test ${test_c} NAME_WE )
  add_executable(${test} ${test_c})
  target_link_libraries( ${test} Io String Math )
  add_test( NAME ${test} COMMAND ${test} )
endforeach( test_c )

if(${verbose_traversal})
  message(STATUS "Exiting Local subdirectory")
endif(${verbose_traversal})

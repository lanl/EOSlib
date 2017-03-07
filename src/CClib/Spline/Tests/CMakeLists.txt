if(${verbose_traversal})
  message(STATUS "Entered Spline/Tests subdirectory")
endif(${verbose_traversal})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB Test_c  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.C")

foreach( test_c ${Test_c} )
  get_filename_component( test ${test_c} NAME_WE )
  add_executable(${test} ${test_c})
  target_link_libraries( ${test} Spline LinkList Io Math String)
endforeach( test_c )


set(BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/Spline)

add_test(NAME TestSpline
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/TestSpline
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/TestSplineOut.blessed
  -D err_blessed=${BLESSED_OUTPUT}/TestSplineErr.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )
set_tests_properties(TestSpline PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")


if(${verbose_traversal})
  message(STATUS "Exiting Spline/Tests subdirectory")
endif(${verbose_traversal})


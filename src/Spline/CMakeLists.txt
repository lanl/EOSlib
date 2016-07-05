if(${verbose_traversal})
  message(STATUS "Entered Spline subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in Spline: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_SPLINE_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/Spline)

set(Spline_src)

add_subdirectory(Subs)

add_library( Spline ${${projectName}_policy} ${Spline_src} )
set_property(TARGET Spline PROPERTY CXX_STANDARD 11)
set_property(TARGET Spline PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Spline PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Spline Io Math )

list( APPEND tests TestSpline )

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Spline)
#  add_test(${test} ${test})
endforeach(test)

#set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro name std")
add_test(NAME TestSpline
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/TestSpline
  -D test_args:string=${args}
  -D out_blessed=${EOSLIB_SPLINE_BLESSED_OUTPUT}/TestSplineOut.blessed
  -D err_blessed=${EOSLIB_SPLINE_BLESSED_OUTPUT}/TestSplineErr.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )
set_tests_properties(TestSpline PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

if(${verbose_traversal})
  message(STATUS "Exiting Spline subdirectory")
endif(${verbose_traversal})

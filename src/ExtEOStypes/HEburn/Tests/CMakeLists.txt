if(${verbose_traversal})
  message(STATUS "Entered HEburn/Tests subdirectory")
endif(${verbose_traversal})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ExtEOStypes/HEburn)


list( APPEND tests TestHEburn2CC TestHEburn TestHEburnCC TestHEeos
                   TestHErate TestIntegrate TestTimeStep )

foreach(test ${tests})
  add_executable(${test}HEburn ${test}.C)
  target_link_libraries(${test}HEburn HEburn)
  # add_test(NAME ${test}HEburn
  #   COMMAND ${CMAKE_COMMAND}
  #   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}HEburn
  #   -D test_args:string=${args}
  #   -D out_blessed=${BLESSED_OUTPUT}/${test}Out.blessed
  #   -D err_blessed=${BLESSED_OUTPUT}/${test}Err.blessed 
  #   -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  #   )
  # message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}HEburn)
  # set_tests_properties(${test}HEburn PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)


if(${verbose_traversal})
  message(STATUS "Exiting HEburn/Tests subdirectory")
endif(${verbose_traversal})

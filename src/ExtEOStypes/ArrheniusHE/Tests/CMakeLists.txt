if(${verbose_traversal})
  message(STATUS "Entered ArrheniusHE/Tests subdirectory")
endif(${verbose_traversal})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ExtEOStypes/ArrheniusHE)


list( APPEND tests TestIdealHE  TestIdealHE_Def  TestIdealHE_Det  TestIdealHug )

foreach(test ${tests})
  add_executable(${test}ArrheniusHE ${test}.C)
  target_link_libraries(${test}ArrheniusHE ArrheniusHE)
  # add_test(NAME ${test}ArrheniusHE
  #   COMMAND ${CMAKE_COMMAND}
  #   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}ArrheniusHE
  #   -D test_args:string=${args}
  #   -D out_blessed=${BLESSED_OUTPUT}/${test}Out.blessed
  #   -D err_blessed=${BLESSED_OUTPUT}/${test}Err.blessed 
  #   -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  #   )
  # message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}ArrheniusHE)
  # set_tests_properties(${test}ArrheniusHE PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)


if(${verbose_traversal})
  message(STATUS "Exiting ArrheniusHE/Tests subdirectory")
endif(${verbose_traversal})

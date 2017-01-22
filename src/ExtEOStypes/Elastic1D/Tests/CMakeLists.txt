if(${verbose_traversal})
  message(STATUS "Entered Elastic1D/Tests subdirectory")
endif(${verbose_traversal})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ExtEOStypes/Elastic1D)

list( APPEND tests TestElastic1D TestElasticPMMA TestEP TestEPshock )

foreach(test ${tests})
  add_executable(${test}Elastic1D ${test}.C)
  target_link_libraries(${test}Elastic1D Elastic1D)
  #add_test(NAME ${test}Elastic1D
  #  COMMAND ${CMAKE_COMMAND}
  #  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}Elastic1D
  #  -D test_args:string=${args}
  #  -D out_blessed=${BLESSED_OUTPUT}/${test}Out.blessed
  #  -D err_blessed=${BLESSED_OUTPUT}/${test}Err.blessed 
  #  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  #  )
  #message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}Elastic1D)
  #set_tests_properties(${test}Elastic1D PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)


if(${verbose_traversal})
  message(STATUS "Exiting Elastic1D/Tests subdirectory")
endif(${verbose_traversal})


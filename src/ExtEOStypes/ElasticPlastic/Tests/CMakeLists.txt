if(${verbose_traversal})
  message(STATUS "Entered ElasticPlastic/Tests subdirectory")
endif(${verbose_traversal})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ExtEOStypes/ElasticPlastic)


list( APPEND tests TestElastic TestEP )
# 
foreach(test ${tests})
   add_executable(${test}ElasticPlastic ${test}.C)
   target_link_libraries(${test}ElasticPlastic ElasticPlastic)
#    add_test(NAME ${test}ElasticPlastic
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}ElasticPlastic
#     -D test_args:string=${args}
#     -D out_blessed=${BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}ElasticPlastic)
#   set_tests_properties(${test}ElasticPlastic PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)


if(${verbose_traversal})
  message(STATUS "Exiting ElasticPlastic/Tests subdirectory")
endif(${verbose_traversal})

if(${verbose_traversal})
  message(STATUS "Entered ElasticPlastic subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in ElasticPlastic: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_ELASTICPLASTIC_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ExtEOStypesElasticPlastic)

set(ElasticPlastic_src)

add_subdirectory(Subs)

add_library( ElasticPlastic ${${projectName}_policy} ${ElasticPlastic_src} )
set_property(TARGET ElasticPlastic PROPERTY CXX_STANDARD 11)
set_property(TARGET ElasticPlastic PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ElasticPlastic PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( ElasticPlastic Io Math )

list( APPEND tests TestElastic TestEP TestRiemann TestXmanager)

foreach(test ${tests})
  add_executable(${test}ElasticPlastic ${test}.C)
  target_link_libraries(${test}ElasticPlastic ElasticPlastic)
 # add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}ElasticPlastic
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_ELASTICPLASTIC_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_ELASTICPLASTIC_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}ElasticPlastic)
  set_tests_properties(${test}ElasticPlastic PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting ElasticPlastic subdirectory")
endif(${verbose_traversal})

if(${verbose_traversal})
  message(STATUS "Entered ElasticPlastic1D subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in ElasticPlastic1D: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_ELASTIC1D_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/ExtEOStypes/Elastic1D)

set(Elastic1D_src)

add_subdirectory(SubsElastic)
add_subdirectory(SubsPlastic)

add_library( Elastic1D ${${projectName}_policy} ${Elastic1D_src} )
set_property(TARGET Elastic1D PROPERTY CXX_STANDARD 11)
set_property(TARGET Elastic1D PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Elastic1D PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Elastic1D Io Math )

list( APPEND tests PMMAhug TestElastic1D TestElasticPMMA TestEP TestEPshock TestRiemann TestVMrate TestXmanager TwoShockEP )

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Elastic1D)
 # add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_ELASTIC1D_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_ELASTIC1D_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test})
  set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting Elastic1D subdirectory")
endif(${verbose_traversal})

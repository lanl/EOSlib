if(${verbose_traversal})
  message(STATUS "Entered GenHayes subdirectory")
endif(${verbose_traversal})

set(EOSLIB_GENHAYES_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/GenHayes)
set(GenHayes_src)

add_subdirectory(SubsVe)
add_subdirectory(SubsVT)

add_library( GenHayes ${${projectName}_policy} ${GenHayes_src} )
set_property(TARGET GenHayes PROPERTY CXX_STANDARD 11)
set_property(TARGET GenHayes PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET GenHayes PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( GenHayes Eos DataBase ODE)

list( APPEND tests Test1  MinP)
list( APPEND testsWithArgs HotSpot Isobar Test2 Test2K Test2VT Test3)

foreach(test ${tests})
  add_executable(${test}GenHayes ${test}.C)
  target_link_libraries(${test}GenHayes GenHayes)
 # add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}GenHayes
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}GenHayes
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_GENHAYES_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_GENHAYES_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}GenHayes)
  set_tests_properties(${test}GenHayes PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

#### use this if test(s) require file specification####

foreach(test ${testsWithArgs})
  add_executable(${test}GenHayes ${test}.C)
  target_link_libraries(${test}GenHayes GenHayes)
 # add_test(${test}IdealGas ${test}IdealGas)
  set(args "file ${EOSLIB_DATA_PATH}/test_data/GenHayesTest.data")
  add_test(NAME ${test}GenHayes
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}GenHayes
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_GENHAYES_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_GENHAYES_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}GenHayes)
  set_tests_properties(${test}GenHayes PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)#
# foreach(test Test2 Test2VT)
#   add_executable(${test}IdealGas ${test}.C)
#   target_link_libraries(${test}IdealGas IdealGas)
#   set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
#   add_test(NAME ${test}IdealGas
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}IdealGas
#     -D test_args:string=${args}
#     -D out_blessed=${EOSLIB_IDEALGAS_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_IDEALGAS_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}IdealGas)
#   set_tests_properties(${test}IdealGas PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting GenHayes subdirectory")
endif(${verbose_traversal})

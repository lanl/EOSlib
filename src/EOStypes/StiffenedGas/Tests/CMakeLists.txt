if(${verbose_traversal})
  message(STATUS "Entered StiffenedGas/Tests subdirectory")
endif(${verbose_traversal})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB Test_c  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.C")

foreach( test_c ${Test_c} )
  get_filename_component( test ${test_c} NAME_WE )
  add_executable(${test} ${test_c})
  target_link_libraries( ${test} StiffenedGas EOS )
endforeach( test_c )

set(BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/StiffenedGas)

# list( APPEND tests Test1 Test2 Test2VT )
# 
# #add_executable(Test1StiffenedGas Test1.C)
# #target_link_libraries(Test1StiffenedGas StiffenedGas)
# #add_test(Test1StiffenedGas Test1StiffenedGas)
# 
# foreach(test Test1)
#   add_executable(${test}StiffenedGas ${test}.C)
#   target_link_libraries(${test}StiffenedGas StiffenedGas)
#   add_test(NAME ${test}StiffenedGas #COMMAND ${test}StiffenedGas
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}StiffenedGas
#     -D test_args:string=${args}
#     -D out_blessed=${BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#     )
#   set_tests_properties(${test}StiffenedGas PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)
# 
# foreach(test Test2 Test2VT)
#   add_executable(${test}StiffenedGas ${test}.C)
#   target_link_libraries(${test}StiffenedGas StiffenedGas)
#   set (args "file ${EOSLIB_DATA_PATH}/test_data/StiffenedGasTest.data")
#   add_test(NAME ${test}StiffenedGas #COMMAND ${test}StiffenedGas
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}StiffenedGas
#     -D test_args:string=${args}
#     -D out_blessed=${BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#     )
#   set_tests_properties(${test}StiffenedGas PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting StiffenedGas/Tests subdirectory")
endif(${verbose_traversal})

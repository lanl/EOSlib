if(${verbose_traversal})
  message(STATUS "Entered Davis subdirectory")
endif(${verbose_traversal})

set(EOSLIB_DAVIS_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/Davis)
set(Davis_src)

add_subdirectory(Subs)

add_library( Davis ${${projectName}_policy} ${Davis_src} )
set_property(TARGET Davis PROPERTY CXX_STANDARD 11)
set_property(TARGET Davis PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Davis PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Davis Eos DataBase ODE)

list( APPEND tests )
list( APPEND testsWithArgs Hugoniot Overdriven TestCJ TestProducts TestReactants)

foreach(test ${tests})
  add_executable(${test}Davis ${test}.C)
  target_link_libraries(${test}Davis Davis)
 # add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}Davis
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}Davis
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_DAVIS_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_DAVIS_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}Davis)
  set_tests_properties(${test}Davis PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

#### use this if test(s) require file specification####

foreach(test ${testsWithArgs})
  add_executable(${test}Davis ${test}.C)
  target_link_libraries(${test}Davis Davis)
  set(args "file ${EOSLIB_DATA_PATH}/test_data/DavisTest.data")
  add_test(NAME ${test}Davis
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}Davis
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_DAVIS_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_DAVIS_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}Davis)
  set_tests_properties(${test}Davis PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
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
  message(STATUS "Exiting Davis subdirectory")
endif(${verbose_traversal})

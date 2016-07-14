if(${verbose_traversal})
  message(STATUS "Entered IdealGas subdirectory")
endif(${verbose_traversal})

set(EOSLIB_IDEALGAS_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/IdealGas)
set(IdealGas_src)

add_subdirectory(SubsVT)
add_subdirectory(SubsVe)

add_library( IdealGas ${${projectName}_policy} ${IdealGas_src} )
set_property(TARGET IdealGas PROPERTY CXX_STANDARD 11)
set_property(TARGET IdealGas PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET IdealGas PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( IdealGas Eos DataBase ODE)

foreach(test IdealGas Test1 TestDetonation TestDeflagration TestPolar)
  add_executable(${test}IdealGas ${test}.C)
  target_link_libraries(${test}IdealGas  IdealGas)
  #  add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}IdealGas
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}IdealGas
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_IDEALGAS_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_IDEALGAS_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}IdealGas)
  set_tests_properties(${test}IdealGas PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

foreach(test Test2 Test2VT)
  add_executable(${test}IdealGas ${test}.C)
  target_link_libraries(${test}IdealGas IdealGas)
  set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}IdealGas
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}IdealGas
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_IDEALGAS_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_IDEALGAS_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}IdealGas)
  set_tests_properties(${test}IdealGas PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting IdealGas subdirectory")
endif(${verbose_traversal})

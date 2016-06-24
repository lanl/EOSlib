if(${verbose_traversal})
  message(STATUS "Entered StiffenedGas subdirectory")
endif(${verbose_traversal})

set(StiffenedGas_src)

add_subdirectory(SubsVT)
add_subdirectory(SubsVe)

add_library( StiffenedGas ${${projectName}_policy} ${StiffenedGas_src} )
set_property(TARGET StiffenedGas PROPERTY CXX_STANDARD 11)
set_property(TARGET StiffenedGas PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET StiffenedGas PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( StiffenedGas Eos DataBase ODE)

list( APPEND tests Test1 Test2 Test2VT )

# Many of these tests output to the terminal. This output should be checked, but isn't.

add_executable(Test1StiffenedGas Test1.C)
target_link_libraries(Test1StiffenedGas StiffenedGas)
add_test(Test1StiffenedGas Test1StiffenedGas)


foreach(test Test2 Test2VT)
  add_executable(${test}StiffenedGas ${test}.C)
  target_link_libraries(${test}StiffenedGas StiffenedGas)
  add_test(NAME ${test}StiffenedGas COMMAND ${test}StiffenedGas
    file ${EOSLIB_DATA_PATH}/test_data/StiffenedGasTest.data)
  set_tests_properties(${test}StiffenedGas PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting StiffenedGas subdirectory")
endif(${verbose_traversal})

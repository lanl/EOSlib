if(${verbose_traversal})
  message(STATUS "Entered IdealGas subdirectory")
endif(${verbose_traversal})

set(IdealGas_src)

add_subdirectory(SubsVT)
add_subdirectory(SubsVe)

add_library( IdealGas ${${projectName}_policy} ${IdealGas_src} )
set_property(TARGET IdealGas PROPERTY CXX_STANDARD 11)
set_property(TARGET IdealGas PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET IdealGas PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( IdealGas Eos DataBase ODE)

list( APPEND tests IdealGas Test1 Test2 Test2VT TestDetonation TestDeflagration TestPolar)

# Many of these tests output to the terminal. This output should be checked, but isn't.

foreach(test IdealGas Test1 TestDetonation TestDeflagration TestPolar)
  add_executable(${test}IdealGas ${test}.C)
  target_link_libraries(${test}IdealGas IdealGas)
  add_test(${test}IdealGas ${test}IdealGas)
endforeach(test)

foreach(test Test2 Test2VT)
  add_executable(${test}IdealGas ${test}.C)
  target_link_libraries(${test}IdealGas IdealGas)
  add_test(NAME ${test}IdealGas COMMAND ${test}IdealGas file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data)
  set_tests_properties(${test}IdealGas PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting IdealGas subdirectory")
endif(${verbose_traversal})

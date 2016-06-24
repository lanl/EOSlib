if(${verbose_traversal})
  message(STATUS "Entered Eos subdirectory")
endif(${verbose_traversal})

set(Eos_src)

add_subdirectory(SubsBase)
add_subdirectory(SubsVT)
add_subdirectory(SubsVe)

add_library( Eos ${${projectName}_policy} ${Eos_src} )
set_property(TARGET Eos PROPERTY CXX_STANDARD 11)
set_property(TARGET Eos PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Eos PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Eos Spline ODE Calc DataBase)

list( APPEND tests Test1 Test2 )

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Eos)
  add_test(${test} ${test})
endforeach(test)

add_executable(Test3 Test3.C)
target_link_libraries(Test3 Eos)
add_test(NAME Test3 COMMAND Test3 file ${EOSLIB_DATA_PATH}/test_data/EosTest.data )
set_tests_properties(Test3 PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

if(${verbose_traversal})
  message(STATUS "Exiting Eos subdirectory")
endif(${verbose_traversal})

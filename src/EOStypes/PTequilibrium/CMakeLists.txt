if(${verbose_traversal})
  message(STATUS "Entered PTequilibrium subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in EOStypes/PTequilibrium: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_PTEQUILIBRIUM_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/PTequilibrium)
set(PTequilibrium_src)

add_subdirectory(Subs)

add_library( PTequilibrium ${${projectName}_policy} ${PTequilibrium_src} )
set_property(TARGET PTequilibrium PROPERTY CXX_STANDARD 11)
set_property(TARGET PTequilibrium PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET PTequilibrium PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( PTequilibrium Eos DataBase ODE)

list( APPEND tests Test1 Test2 Test2VT TestApproxT TestSpline1 TestSpline2 TestSpline2VT TestSplineParams TestSplinePe1 TestSplinePe2 TestSplinePMMA )

foreach(test ${tests})
  add_executable(${test}PTequilibrium ${test}.C)
  target_link_libraries(${test}PTequilibrium PTequilibrium)
 # add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}PTequilibrium
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}PTequilibrium
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_PTEQUILIBRIUM_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_PTEQUILIBRIUM_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}PTequilibrium)
  set_tests_properties(${test}PTequilibrium PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

#### use this if test(s) require file specification####
#
# foreach(test Test2 Test2VT)
#   add_executable(${test}IdealGas ${test}.C)
#   target_link_libraries(${test}IdealGas IdealGas)
#   set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
#   add_test(NAME ${test}IdealGas
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}IdealGas
#     -D test_args:string=${args}
#     -D out_blessed=${EOSLIB_EQUILIBRIUM_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_EQUILIBRIUM_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}IdealGas)
#   set_tests_properties(${test}IdealGas PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting PTequilibrium subdirectory")
endif(${verbose_traversal})


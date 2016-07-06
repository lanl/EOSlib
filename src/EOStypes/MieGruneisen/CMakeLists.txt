if(${verbose_traversal})
  message(STATUS "Entered MieGruneisen subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in EOStypes/MieGruneisen: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_MIEGRUNEISEN_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/MieGruneisen)
set(MieGruneisen_src)

add_subdirectory(Subs)

add_library( MieGruneisen ${${projectName}_policy} ${MieGruneisen_src} )
set_property(TARGET MieGruneisen PROPERTY CXX_STANDARD 11)
set_property(TARGET MieGruneisen PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET MieGruneisen PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( MieGruneisen Eos DataBase ODE Calc ${CMAKE_DL_LIBS})

list( APPEND tests Test1 Test2 Test2VT TestApproxT TestSpline1 TestSpline2 TestSpline2VT TestSplineParams TestSplinePe1 TestSplinePe2 TestSplinePMMA )

foreach(test ${tests})
  add_executable(${test}MieGruneisen ${test}.C)
  target_link_libraries(${test}MieGruneisen MieGruneisen)
 # add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}MieGruneisen
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}MieGruneisen
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_MIEGRUNEISEN_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_MIEGRUNEISEN_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}MieGruneisen)
  set_tests_properties(${test}MieGruneisen PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
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
#     -D out_blessed=${EOSLIB_IDEALGAS_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_IDEALGAS_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}IdealGas)
#   set_tests_properties(${test}IdealGas PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting MieGruneisen subdirectory")
endif(${verbose_traversal})


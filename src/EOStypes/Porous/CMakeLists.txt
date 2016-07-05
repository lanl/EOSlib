if(${verbose_traversal})
  message(STATUS "Entered Porous subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in EOStypes/Porous: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_POROUS_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/Porous)
set(Porous_src)

add_subdirectory(SubsEOS)
add_subdirectory(SubsPhi)

add_library( Porous ${${projectName}_policy} ${Porous_src} )
set_property(TARGET Porous PROPERTY CXX_STANDARD 11)
set_property(TARGET Porous PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Porous PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Porous Eos DataBase ODE)

list( APPEND tests TestFrozenPorous TestPhi TestPorousEOS )

foreach(test ${tests})
  add_executable(${test}Porous ${test}.C)
  target_link_libraries(${test}Porous Porous)
 # add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}Porous
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}Porous
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_POROUS_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_POROUS_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}Porous)
  set_tests_properties(${test}Porous PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
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
  message(STATUS "Exiting Porous subdirectory")
endif(${verbose_traversal})


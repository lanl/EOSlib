if(${verbose_traversal})
  message(STATUS "Entered Uniaxial subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in EOStypes/Uniaxial: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_UNIAXIAL_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/Uniaxial)
set(Uniaxial_src)

add_subdirectory(Subs)

add_library( Uniaxial ${${projectName}_policy} ${Uniaxial_src} )
set_property(TARGET Uniaxial PROPERTY CXX_STANDARD 11)
set_property(TARGET Uniaxial PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Uniaxial PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Uniaxial Eos DataBase ODE)

list( APPEND tests VonMisesTest )

foreach(test ${tests})
  add_executable(${test}Uniaxial ${test}.C)
  target_link_libraries(${test}Uniaxial Uniaxial)
 # add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}Uniaxial
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}Uniaxial
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_UNIAXIAL_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_UNIAXIAL_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}Uniaxial)
  set_tests_properties(${test}Uniaxial PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
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
  message(STATUS "Exiting Uniaxial subdirectory")
endif(${verbose_traversal})


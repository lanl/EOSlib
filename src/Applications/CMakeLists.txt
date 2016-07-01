if(${verbose_traversal})
  message(STATUS "Entered Applications subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in Applications: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_APPLICATIONS_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/Applications)
set(Applications_src)

add_subdirectory(Subs)

add_library(  ${${projectName}_policy} ${Applications_src} )
set_property(TARGET Applications PROPERTY CXX_STANDARD 11)
set_property(TARGET Applications PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Applications PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Applications Eos DataBase ODE)

list( APPEND tests TestXisentrope TestXmanager TestXRiemann)

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Applications)
 # add_test(${test}IdealGas ${test}IdealGas)
 # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
  add_test(NAME ${test}
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_DAVIS_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_DAVIS_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS Applications ${CMAKE_CURRENT_BINARY_DIR}/${test})
  set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
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
  message(STATUS "Exiting Applications subdirectory")
endif(${verbose_traversal})


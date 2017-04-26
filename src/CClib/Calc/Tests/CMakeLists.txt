if(${verbose_traversal})
  message(STATUS "Entered Calc/Tests subdirectory")
endif(${verbose_traversal})


# set(BLESSED_OUTPUT "${EOSLIB_BLESSED_OUTPUT}/Calc")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
# 
# set( test "TestCalc" )
# add_executable(${test} ${test}.C)
# target_link_libraries( ${test} Calc LinkList Io Math String )
# 
# # TestCalc requires interactive input.
# #set(args "echo -e 'm(2,2)\nrho\nabc'") #this line provides correct output from terminal
# set(args "rho abc m(2,2)")
# add_test( NAME ${test}
#   COMMAND ${CMAKE_COMMAND}
#   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
#   -D test_args:string=${args}
#   -D out_blessed=${BLESSED_OUTPUT}/${test}Out.blessed
#   -D err_blessed=${BLESSED_OUTPUT}/${test}Err.blessed 
#   -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
# )
# set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")


if(${verbose_traversal})
  message(STATUS "Exiting Calc/Tests subdirectory")
endif(${verbose_traversal})


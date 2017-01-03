if(${verbose_traversal})
  message(STATUS "Entered Calc/Tests subdirectory")
endif(${verbose_traversal})


message(STATUS "blessed output in Calc: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_CALC_BLESSED_OUTPUT "${EOSLIB_BLESSED_OUTPUT}/Calc")
# To Do
# 1. move ${EOSLIB_BLESSED_OUTPUT}/Calc to subdirector blessed_output
#    files out.test and optionally err.test and EOSlog
# 2. for EOS tests, add subdirectory DATA for test data
# 3. modify compare_outputs.cmake to compare only file in blessed_output
#    most test do not need err.test or EOSlog

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")


set( test "TestCalc" )
add_executable(${test} ${test}.C)
target_link_libraries( ${test} Calc LinkList Io Math String)
target_include_directories(${test} BEFORE PUBLIC
                                   "${CMAKE_CURRENT_SOURCE_DIR}/..")

# TestCalc requires interactive input.
#set(args "echo -e 'm(2,2)\nrho\nabc'") #this line provides correct output from terminal
set(args "rho abc m(2,2)")
add_test( NAME ${test}
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
  -D test_args:string=${args}
  -D out_blessed=${EOSLIB_CALC_BLESSED_OUTPUT}/${test}Out.blessed
  -D err_blessed=${EOSLIB_CALC_BLESSED_OUTPUT}/${test}Err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
)
set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

message(STATUS Calc ${CMAKE_CURRENT_BINARY_DIR}/${test})




if(${verbose_traversal})
  message(STATUS "Exiting Calc/Tests subdirectory")
endif(${verbose_traversal})


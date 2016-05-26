# Takes (as CMAKE variables) the arguments:
# - test_cmd: test program
# - output_blessed: known "good" test output
# - output_test: name of the output file test_cmd will produce

if(NOT test_cmd)
  message(FATAL_ERROR "Variable test_cmd not defined")
endif(NOT test_cmd)

if(NOT output_blessed)
  message(FATAL_ERROR "Variable output_blessed not defined")
endif(NOT output_blessed)

if(NOT output_test)
  message(FATAL_ERROR "Variable output_test not defined")
endif(NOT output_test)

execute_process(
  COMMAND ${test_cmd}
  COMMAND ${CMAKE_COMMAND} -E compare_files ${output_blessed} ${output_test}
  RESULT_VARIABLE test_not_successful
  )

if(test_not_successful)
  message(SEND_ERROR "${output_test} does not match ${output_blessed}!")
endif(test_not_successful)

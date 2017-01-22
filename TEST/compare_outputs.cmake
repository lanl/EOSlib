# Takes (as CMAKE variables) the arguments:
# - test_cmd: test program
# - test_args: args for the test program
# - out_blessed: known "good" test output
# - err_blessed: known "good" test err

if(NOT test_cmd)
  message(FATAL_ERROR "Variable test_cmd not defined")
endif(NOT test_cmd)

if(NOT out_blessed)
  message(FATAL_ERROR "Variable out_blessed not defined")
endif(NOT out_blessed)

if(NOT err_blessed)
  message(FATAL_ERROR "Variable err_blessed not defined")
endif(NOT err_blessed)

set(OUT_TEST out.test)
set(ERR_TEST err.test)

separate_arguments(test_args)
message(STATUS ${test_args})

# directory Testing created by ctest
# ToDo: mirror subdir structure in blessed_output directory
set(CWD ${CMAKE_CURRENT_LIST_DIR}/../Testing)

execute_process(
  COMMAND ${test_cmd} ${test_args}
  OUTPUT_FILE ${OUT_TEST}
  ERROR_FILE ${ERR_TEST}
  RESULT_VARIABLE test_failed_to_run
  WORKING_DIRECTORY ${CWD}
  )
#set(EOSlog ${CMAKE_BINARY_DIR}/EOSlog)
#if( EXISTS "${EOSlog}" )
#  file(RENAME ${EOSlog} ${CMAKE_CURRENT_LIST_DIR}/EOSlog)
#endif()

if(test_failed_to_run)
  message(SEND_ERROR "${test_cmd} failed to run successfully!")
  return(test_failed_to_run)
endif(test_failed_to_run)

set(OUT_TEST ${CWD}/out.test)
set(ERR_TEST ${CWD}/err.test)

execute_process(
 COMMAND ${CMAKE_COMMAND} -E compare_files ${out_blessed} ${OUT_TEST}
#  COMMAND ${CMAKE_COMMAND} -E compare_files ${err_blessed} ${ERR_TEST}
  RESULT_VARIABLE test_not_successful
  )

#file(REMOVE ${OUT_TEST} ${ERR_TEST})

if(test_not_successful)
  message(SEND_ERROR "${OUT_TEST} does not match ${out_blessed}!")
endif(test_not_successful)

return(test_not_successful)
